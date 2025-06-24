#include <Arduino.h>
#include "IRCounter.h"
#include <WiFi.h>
//#include <ESPAsyncWebServer.h>
//#include <AsyncTCP.h>
//#include <ArduinoJson.h>
//#include <FS.h>
//#include <LittleFS.h>
//#include "IRCounter.h"

#define IR1_PIN D7
#define IR2_PIN D1
#define LED1_PIN D8
#define LED2_PIN D5

IRCounter capteur1(IR1_PIN, LED1_PIN, "Capteur 1");
IRCounter capteur2(IR2_PIN, LED2_PIN, "Capteur 2");

IRCounter* IRCounter::instance1 = &capteur1;
IRCounter* IRCounter::instance2 = &capteur2;

int seuilDetection = 100;

//AsyncWebServer server(80);
//AsyncWebSocket ws("/ws");

/*
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    String msg = "";
    for (size_t i = 0; i < len; i++) msg += (char)data[i];

    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, msg);
    if (!error && doc["type"] == "config") {
      seuilDetection = doc["threshold"];
      Serial.printf("Nouveau seuil de détection: %d ms\n", seuilDetection);
    }
  }
}
*/
void setup() {
  Serial.begin(115200);
  delay(1000); // Attente pour la connexion série
  //LittleFS.begin();
  //WiFi.softAP("ESP_Config");

  capteur1.begin();
  capteur2.begin();

  attachInterrupt(digitalPinToInterrupt(IR1_PIN), IRCounter::isr1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IR2_PIN), IRCounter::isr2, CHANGE);

  //server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  //ws.onEvent(onWsEvent);
  //server.addHandler(&ws);
  //server.begin();

}

void loop() {
  capteur1.update();
  capteur2.update();
}
