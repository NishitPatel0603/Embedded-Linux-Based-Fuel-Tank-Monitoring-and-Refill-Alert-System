#include <mosquitto.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TB_BROKER "mqtt.eu.thingsboard.cloud"
#define TB_PORT   1883

#define TB_TOKEN  "xO4qYYJGHKnDFQaDKTba"

#define TB_TOPIC  "v1/devices/me/telemetry"

int main(void)
{
    setbuf(stdout, NULL);   

    struct mosquitto *mosq;
    FILE *fp;
    char payload[128];

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        printf("Mosquitto init failed\n");
        return -1;
    }

   
    mosquitto_username_pw_set(mosq, TB_TOKEN, NULL);

    if (mosquitto_connect(mosq, TB_BROKER, TB_PORT, 60) != MOSQ_ERR_SUCCESS) {
        printf("ThingsBoard connect failed\n");
        return -1;
    }

    printf("Connected to ThingsBoard Cloud\n");

    while (1) {
        fp = fopen("/tmp/sensor_data.txt", "r");
        if (fp) {
            if (fgets(payload, sizeof(payload), fp)) {

                mosquitto_publish(
                    mosq,
                    NULL,
                    TB_TOPIC,
                    strlen(payload),
                    payload,
                    1,      
                    false
                );

                printf("Sent to ThingsBoard: %s", payload);
            }
            fclose(fp);
        }

        mosquitto_loop(mosq, 1000, 1);
        sleep(1);
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}

