#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
// ==== CONFIG ====
#define IR_PIN D7  // GPIO13

const char* ssid = "NETGEAREA758B-g026";
const char* password = "boulerouge62219!";
const char* mqtt_server = "192.168.1.14";
const int mqtt_port = 1883;
const char* mqtt_topic = "test";
volatile bool signalState = HIGH;
unsigned long lastChangeTime = millis();

unsigned long signalLowStart = 0;
unsigned long signalHighStart = 0;
bool beamBroken = false;
bool passageCounted = false;

unsigned long passageCount = 0;

// ==== OBJETS ====
WiFiClient espClient;
PubSubClient client(espClient);
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // UTC
// ==== WIFI ====
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connexion WiFi à...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(ssid);
}

// ==== MQTT ====
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connexion MQTT...");
    if (client.connect("ESP8266Compteur")) {
      Serial.println(" connectée");
    } else {
      Serial.print("Erreur: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
// ==== PUBLICATION GROUPEE ====
void publishGroupedPassages() {
  String payload = "{";
  payload += "\"passages\":" + String(passageCount) ;
  payload += "}";

  Serial.println("Envoi groupé MQTT : " + payload);
  client.publish(mqtt_topic, payload.c_str());
}
void IRAM_ATTR handleIRChange() {
  signalState = digitalRead(IR_PIN); // Mémorise l'état de la sortie de tsop suite à un changement d'état
  lastChangeTime = millis(); // Déclenche un chrono
}
// ==== SETUP ====
void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);
  pinMode(D8, OUTPUT);
  Serial.println(" ");
  Serial.println("Recepteur de comptage");
  attachInterrupt(digitalPinToInterrupt(IR_PIN), handleIRChange, CHANGE);
  delay(1000);
  
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
}
// ==== LOOP ====
void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  //timeClient.update();
  delay(10);
  // Faisceau coupé : niveau haut stable > 300 ms 
if ( signalState == HIGH){
  //Serial.println("faisceau rompu");
  signed long temps = millis() - lastChangeTime;
  if ( temps > 100  )
  {
    beamBroken = true;
    signalHighStart = millis();
        }
  }

  // Faisceau rétabli > 800 ms après coupure : compter un passage
  if (beamBroken) {
  
    if ((millis() - signalHighStart) > 400 ) {
      passageCount++;
      passageCounted = true;
      beamBroken = false;
      Serial.print("Passage détecté. Total : ");
      Serial.println(passageCount);
      publishGroupedPassages();
    }
  }
  //delay(10); // petit délai pour éviter surcharge CPU
  if (digitalRead(D7))
    {
      digitalWrite(D8, LOW);
    }
    else
    { 
      digitalWrite(D8, HIGH);
    }

}
