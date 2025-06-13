#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ========== Wi-Fi Configuration ==========
const char* ssid = "NETGEAREA758B-g026";
const char* password = "boulerouge62219!";

// ========== MQTT Configuration ==========
const char* mqtt_server = "192.168.1.10";
const int mqttPort = 1883;

// ========== NTP Configuration ==========
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);  // UTC+1 offset (3600s)

// ========== Weekdays and Months ==========
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// ========== Wi-Fi, MQTT Clients ==========
ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);

// ========== Setup ==========
void setup() {
  Serial.begin(115200);

  setup_wifi();
  setup_mqtt();
  timeClient.begin();

  // Publier un message initial
  client.publish("test", "Hello from ESP8266 with NTP!");
}

// ========== Main Loop ==========
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  timeClient.update();
  publishTime();

  delay(10000);  // publier toutes les 10 secondes
}

// ========== Setup Wi-Fi ==========
void setup_wifi() {
  WiFiMulti.addAP(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ========== Setup MQTT ==========
void setup_mqtt() {
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
  reconnect();
}

// ========== MQTT Callback ==========
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur [");
  Serial.print(topic);
  Serial.print("] : ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  client.publish("test", "ACK from ESP8266");
}

// ========== MQTT Reconnect ==========
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    String clientId = "ESP8266Client-" + String(WiFi.macAddress());
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT");
      client.subscribe("test");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 2 seconds");
      delay(2000);
    }
  }
}

// ========== Publish Time to MQTT ==========
void publishTime() {
  time_t epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  String weekDay = weekDays[timeClient.getDay()];
  struct tm* ptm = gmtime((time_t*)&epochTime);

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;

  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  String fullTimeMessage = "Time: " + formattedTime + ", Date: " + currentDate + ", Day: " + weekDay;

  Serial.println(fullTimeMessage);
  client.publish("time", fullTimeMessage.c_str());
}
