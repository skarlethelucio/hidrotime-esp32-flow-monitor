# Hidrotime - ESP32 Water Monitoring System

ESP32-based water monitoring system designed for real-time measurement and leak detection.

---

Hidrotime is an IoT system designed to monitor water levels and flow in a tank using an ESP32 and sensors.

The system not only provides real-time data through a REST API, but is also designed to help detect abnormal water consumption patterns, enabling early leak detection and improving water efficiency.

---

##  Features
- Real-time water level monitoring
- Flow measurement using pulse sensor
- Leak detection based on abnormal flow behavior
- REST API for data access
- Embedded system implementation
- Designed for mobile integration

---

##  System Components
- ESP32
- Arduino IDE
- WiFi (IoT communication)
- ArduinoJson

---

## ⚠️ Requirements
To compile and run this project you need:

- ESP32 board  
- Arduino IDE with ESP32 support installed  

### Required libraries:
- WiFi.h (included with ESP32 core)  
- WebServer.h  
- ArduinoJson  

---

## 🌐 API

### Endpoint
GET /tankStatus


### Example Output
Cuando accedas al endpoint, recibirás un objeto JSON estructurado de la siguiente manera:

```json
{
  "waterColumn": 75.2,
  "level": 50,
  "volume": 712,
  "flowTotal": 3.45
}

```
---

##  Testing

Open your browser and go to:

 ```http://<ESP32_IP>/tankStatus ```


## Mobile Integration

This system was designed to integrate with a mobile application developed in Android Studio for real-time monitoring.






Author : Ammy Lucio




