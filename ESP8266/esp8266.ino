#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ThingSpeak.h>
#include "secrets.h"
#include "webpage.h"

// Wiring: Arduino pin 3 -> Wemos D5 | Arduino pin 12 -> Wemos D6 | GND -> GND
SoftwareSerial arSerial(D5, D6);

ESP8266WebServer server(80);
WiFiClient tsClient;

float g_temp = 0.0, g_humidity = 0.0;
int g_smoke = 0, g_flame = 0, g_waterLevel = 100, g_pir = 0, g_ldr = 512;
int g_pump = 0, g_fan = 0, g_buzzer = 0, g_servo = 90;

String serialBuf = "";
unsigned long lastTS = 0;
const unsigned long TS_INTERVAL = 20000;

void readArduinoSerial() {
  while (arSerial.available()) {
    char c = arSerial.read();
    if (c == '\n') {
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, serialBuf)) {
        if (doc.containsKey("flame"))      g_flame      = doc["flame"];
        if (doc.containsKey("smoke"))      g_smoke      = doc["smoke"];
        if (doc.containsKey("temp"))       g_temp       = doc["temp"];
        if (doc.containsKey("humidity"))   g_humidity   = doc["humidity"];
        if (doc.containsKey("waterLevel")) g_waterLevel = doc["waterLevel"];
        if (doc.containsKey("pir"))        g_pir        = doc["pir"];
        if (doc.containsKey("ldr"))        g_ldr        = doc["ldr"];
      }
      serialBuf = "";
    } else {
      serialBuf += c;
      if (serialBuf.length() > 300) serialBuf = "";
    }
  }
}

void handleRoot() {
  size_t totalLen = strlen_P(INDEX_HTML);
  server.setContentLength(totalLen);
  server.send(200, "text/html", "");
  const size_t CHUNK = 1024;
  char buf[CHUNK + 1];
  size_t offset = 0;
  while (offset < totalLen) {
    size_t toSend = min(CHUNK, totalLen - offset);
    memcpy_P(buf, INDEX_HTML + offset, toSend);
    buf[toSend] = '\0';
    server.sendContent(buf);
    offset += toSend;
  }
}

void handleData() {
  StaticJsonDocument<256> doc;
  doc["flame"] = g_flame;
  doc["smoke"] = g_smoke;
  doc["temp"] = g_temp;
  doc["humidity"] = g_humidity;
  doc["waterLevel"] = g_waterLevel;
  doc["pir"] = g_pir;
  doc["ldr"] = g_ldr;
  doc["pump"] = g_pump;
  doc["fan"] = g_fan;
  doc["servo"] = g_servo;
  doc["buzzer"] = g_buzzer;
  String json;
  serializeJson(doc, json);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleControl() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"err\":\"no body\"}");
    return;
  }
  StaticJsonDocument<64> doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"err\":\"invalid json\"}");
    return;
  }
  if (doc.containsKey("pump"))   g_pump   = doc["pump"];
  if (doc.containsKey("fan"))    g_fan    = doc["fan"];
  if (doc.containsKey("buzzer")) g_buzzer = doc["buzzer"];
  if (doc.containsKey("servo"))  g_servo  = doc["servo"];

  String cmd;
  serializeJson(doc, cmd);
  arSerial.println(cmd);

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"ok\":1}");
}

void handleCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

void setup() {
  Serial.begin(115200);
  arSerial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  ThingSpeak.begin(tsClient);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/control", HTTP_POST, handleControl);
  server.on("/control", HTTP_OPTIONS, handleCORS);
  server.on("/data", HTTP_OPTIONS, handleCORS);
  server.begin();
  Serial.println("Server started.");
}

void loop() {
  server.handleClient();
  readArduinoSerial();

  if (millis() - lastTS >= TS_INTERVAL) {
    lastTS = millis();
    if (WiFi.status() == WL_CONNECTED) {
      ThingSpeak.setField(1, g_temp);
      ThingSpeak.setField(2, g_humidity);
      ThingSpeak.setField(3, g_smoke);
      ThingSpeak.setField(4, g_flame);
      ThingSpeak.setField(5, g_waterLevel);
      //ThingSpeak.setField(6, g_pir);
      ThingSpeak.setField(7, g_ldr);
      int res = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
      Serial.println("ThingSpeak: " + String(res == 200 ? "OK" : "FAIL (" + String(res) + ")"));
    }
  }
}
