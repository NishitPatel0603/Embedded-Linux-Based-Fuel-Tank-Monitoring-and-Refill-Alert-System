#!/bin/sh

echo "Starting sensor app..."
sensor_app &
SENSOR_PID=$!

sleep 1

echo "Starting MQTT publisher..."
mqtt_pub &
MQTT_PID=$!

echo "System running. Press Ctrl+C to stop."

# Handle Ctrl+C
trap 'echo "Stopping..."; kill $SENSOR_PID $MQTT_PID; exit 0' INT

# Wait forever
while true; do
    sleep 1
done
