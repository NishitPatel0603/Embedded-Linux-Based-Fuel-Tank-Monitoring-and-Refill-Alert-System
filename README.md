# Embedded Linux Based Fuel Tank Monitoring and Refill Alert System

This repository contains a complete **Yocto-based Embedded Linux project**
for monitoring fuel tank levels and generating refill alerts.
The project integrates custom application code with the Yocto Project build system.

---

## 📁 Repository Structure


---

## 🧠 Project Overview

The system is designed to:
- Monitor fuel tank levels using sensors
- Process sensor data on an Embedded Linux platform
- Generate alerts when fuel level is below a threshold
- Support integration with IoT / MQTT-based systems (optional)

The build system is based on **Yocto Project (Kirkstone branch)**.

---

## 🛠 Application Code (`codes/`)

The `codes/` directory contains user-space applications and scripts, such as:
- Sensor data collection
- MQTT publish/communication logic
- Startup scripts for system initialization

Example:
- `sensor_app.c`
- `mqtt_pub.c`
- `start_system.sh`

---

## 🧱 Yocto Layers

### `poky/`
- Core Yocto Project reference distribution
- Provides BitBake, OpenEmbedded-Core, and build tools

### `meta-openembedded/`
- Community-maintained OpenEmbedded layers
- Provides additional recipes and packages (multimedia, networking, etc.)

---

## ⚙️ Build Instructions (Basic)

> Prerequisite: Linux host system with required Yocto dependencies installed.

```bash
cd poky
source oe-init-build-env
Add layers (if not already added):

bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-python


Build an image:

bitbake core-image-minimal
