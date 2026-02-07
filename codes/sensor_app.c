#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/stat.h>

#define I2C_BUS   "/dev/i2c-2"
#define MUX_ADDR  0x70
#define VL53_ADDR 0x29
#define FILTER_N  5

#define TEST_MAX_DISTANCE_MM 1000   // 1 meter testing range

void i2c_select(int fd, uint8_t addr)
{
    ioctl(fd, I2C_SLAVE, addr);
}

void i2c_write_reg(int fd, uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    i2c_select(fd, addr);
    write(fd, buf, 2);
}

uint16_t i2c_read_reg16(int fd, uint8_t addr, uint8_t reg)
{
    uint8_t v[2];
    i2c_select(fd, addr);
    write(fd, &reg, 1);
    read(fd, v, 2);
    return (v[0] << 8) | v[1];
}

void mux_select(int fd, uint8_t ch)
{
    uint8_t v = (1 << ch);
    i2c_select(fd, MUX_ADDR);
    write(fd, &v, 1);
    usleep(5000);
}

void vl53_init(int fd)
{
    i2c_write_reg(fd, VL53_ADDR, 0x88, 0x00);
    i2c_write_reg(fd, VL53_ADDR, 0x80, 0x01);
    i2c_write_reg(fd, VL53_ADDR, 0xFF, 0x01);
    i2c_write_reg(fd, VL53_ADDR, 0x00, 0x00);
    i2c_write_reg(fd, VL53_ADDR, 0x91, 0x3c);
    i2c_write_reg(fd, VL53_ADDR, 0x00, 0x01);
    i2c_write_reg(fd, VL53_ADDR, 0xFF, 0x00);
    i2c_write_reg(fd, VL53_ADDR, 0x80, 0x00);
}

uint16_t vl53_read_distance(int fd)
{
    i2c_write_reg(fd, VL53_ADDR, 0x00, 0x01);
    usleep(50000);
    return i2c_read_reg16(fd, VL53_ADDR, 0x1E);
}

uint16_t avg(uint16_t *b)
{
    uint32_t s = 0;
    for (int i = 0; i < FILTER_N; i++)
        s += b[i];
    return s / FILTER_N;
}

/* INVERTED LOGIC:
   Near = 100%
   Far (1m) = 0%
*/
int distance_to_percent(uint16_t dist)
{
    if (dist >= TEST_MAX_DISTANCE_MM)
        return 0;

    return 100 - ((dist * 100) / TEST_MAX_DISTANCE_MM);
}

int main(void)
{
    setbuf(stdout, NULL);
    mkdir("/tmp", 0777);

    int fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) {
        perror("I2C open failed");
        return 1;
    }

    FILE *fp;
    uint16_t s1[FILTER_N] = {0};
    uint16_t s2[FILTER_N] = {0};
    int i = 0;

    mux_select(fd, 0);
    vl53_init(fd);
    mux_select(fd, 1);
    vl53_init(fd);

    while (1) {
        mux_select(fd, 0);
        s1[i] = vl53_read_distance(fd);

        mux_select(fd, 1);
        s2[i] = vl53_read_distance(fd);

        uint16_t d1 = avg(s1);
        uint16_t d2 = avg(s2);

        int p1 = distance_to_percent(d1);
        int p2 = distance_to_percent(d2);

        printf("Sensor1: %d%% | Sensor2: %d%%\n", p1, p2);

        fp = fopen("/tmp/sensor_data.txt", "w");
        if (fp) {
            fprintf(fp,
                "{\"sensor1\": %d, \"sensor2\": %d}\n",
                p1, p2);
            fclose(fp);
        }

        i = (i + 1) % FILTER_N;
        sleep(1);
    }
}

