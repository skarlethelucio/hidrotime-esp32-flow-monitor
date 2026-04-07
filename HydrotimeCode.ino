/*********
Description: IoT device for water tank monitoring using a 4-20 mA level sensor.

REST API:
- /tankStatus

Description:
Provides real-time tank data including:
- Water column height (cm)
- Fill percentage (%)
- Water volume (liters)

The system is designed for a cylindrical tank and calculates volume based on tank dimensions.

GET /tankStatus

REST IN:
None

REST OUT:
- waterColumn [float] → Water level height (cm)
- level [int] → Tank fill percentage (0–100%)
- volume [int] → Water volume in liters
- flowTotal [float] → Total accumulated flow

Example:

Status: 200 OK

{
  "waterColumn": 75.2,
  "level": 50,
  "volume": 712,
  "flowTotal": 3.45
}
*********/

/****** Libraries *****/
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

/****** Global Variables *****/
volatile uint32_t pulseCount = 0;
float waterflow = 0;

float floatLevelCm = 0.0;
float floatCapacityCm = 150.0;
int intLevelPercent = 0;
int intVolume = 0;

const int intTankRadiusCm = 55;
float floatLitersPerCm = 0.0;

/****** Network Configuration *****/
const char* ssid = "Universidad";
const char* password = "";

IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 200);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

StaticJsonDocument<300> jsonDocument;
char bufferJson[300];

/****** Pins Definition *****/
#define LEVEL_SENSOR_PIN 34
#define FLOW_SENSOR_PIN 27

/****** Interrupt ********/
void IRAM_ATTR pulse() {
  pulseCount++;
}

/****** Setup ********/
void setup() {
  Serial.begin(115200);

  pinMode(LEVEL_SENSOR_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulse, RISING);

  // Cálculo de litros por cm
  floatLitersPerCm = (PI * sq(intTankRadiusCm)) / 1000.0;

  Serial.print("Litros por cm: ");
  Serial.println(floatLitersPerCm);

  // Configuración WiFi
  if (!WiFi.config(ip, gateway, subnet)) {
    Serial.println("Error configurando IP estática");
  }

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
  } else {
    Serial.println("\nNo se pudo conectar a WiFi");
  }

  // Endpoint API
  server.on("/tankStatus", getTankStatus);
  server.begin();
}

/****** Loop ********/
void loop() {
  server.handleClient();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 2000) {
    readSensors();
    lastCheck = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Reconectando WiFi...");
      WiFi.begin(ssid, password);
    }
  }
}

/****** Sensor Reading ********/
int readAverageADC(int pin) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 10;
}

void readSensors() {
  int rawADC = readAverageADC(LEVEL_SENSOR_PIN);
  float voltage = (rawADC / 4095.0) * 3.3;

  if (voltage < 0.6) {
    floatLevelCm = 0;
  } else {
    floatLevelCm = 83.33 * (voltage - 0.6);
  }

  floatLevelCm = constrain(floatLevelCm, 0, floatCapacityCm);

  intLevelPercent = (floatLevelCm / floatCapacityCm) * 100;
  intLevelPercent = constrain(intLevelPercent, 0, 100);

  intVolume = floatLevelCm * floatLitersPerCm;

  waterflow = pulseCount / 450.0;

  Serial.printf("ADC: %d | Volts: %.2fV | Nivel: %.1fcm | Vol: %dL | Flow: %.2f\n",
                rawADC, voltage, floatLevelCm, intVolume, waterflow);
}

/****** API ********/
void getTankStatus() {
  jsonDocument.clear();

  jsonDocument["waterColumn"] = floatLevelCm;
  jsonDocument["level"] = intLevelPercent;
  jsonDocument["volume"] = intVolume;
  jsonDocument["flowTotal"] = waterflow;

  serializeJson(jsonDocument, bufferJson);
  server.send(200, "application/json", bufferJson);
}
