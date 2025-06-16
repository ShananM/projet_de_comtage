#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SSID_ADDR 0
#define PASS_ADDR 32

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);  // UTC+1

const char* mqtt_server = "192.168.1.11";
const int mqttPort = 1883;

String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// ========== EEPROM ==========
void EEPROMWriteString(int addr, const String& data, int maxLen) {
  for (int i = 0; i < maxLen; i++) {
    EEPROM.write(addr + i, i < data.length() ? data[i] : 0);
  }
  EEPROM.commit();
}

String EEPROMReadString(int addr, int maxLen) {
  char data[maxLen + 1];
  for (int i = 0; i < maxLen; i++) {
    data[i] = EEPROM.read(addr + i);
    if (data[i] == 0) break;
  }
  data[maxLen] = 0;
  return String(data);
}

// ========== Mode configuration ==========
void mode_config() {
  WiFi.softAP("ESP_Config");
  Serial.println("⚙️ Mode config activé. Connectez-vous à 'ESP_Config'.");
  Serial.print(" Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  server.begin();
}

// ========== Connexion WiFi ==========
void connectToWiFi(const String& ssid, const String& password) {
  WiFi.begin(ssid, password);
  Serial.println(" Tentative de connexion au WiFi...");
  
  int timeout = 10000;
  int start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n Connecté au WiFi !");
    Serial.print(" IP : ");
    Serial.println(WiFi.localIP());

    EEPROMWriteString(SSID_ADDR, ssid, 32);
    EEPROMWriteString(PASS_ADDR, password, 64);

    WiFi.softAPdisconnect(true);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    client.setServer(mqtt_server, mqttPort);
    client.setCallback(callback);
    timeClient.begin();
  } else {
    Serial.println("\n Échec de connexion WiFi. Retour en mode config.");
    EEPROMWriteString(SSID_ADDR, "", 32);
    EEPROMWriteString(PASS_ADDR, "", 64);
    mode_config();
  }
}

// ========== WebSocket ==========
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    String json = String((char*)payload);
    StaticJsonDocument<256> doc;
    if (deserializeJson(doc, json)) {
      Serial.println("⚠️ Erreur JSON");
      return;
    }

    if (doc.containsKey("ssid") && doc.containsKey("password")) {
      connectToWiFi(doc["ssid"].as<String>(), doc["password"].as<String>());
    } else if (doc.containsKey("commande") && doc["commande"] == "reset") {
      WiFi.disconnect(true);
      EEPROMWriteString(SSID_ADDR, "", 32);
      EEPROMWriteString(PASS_ADDR, "", 64);
      mode_config();
    } else {
      Serial.println(" Commande inconnue via WebSocket.");
    }
  }
}

// ========== MQTT ==========
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(" Message [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();
  client.publish("test", "ACK from ESP8266");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println(" Connexion à MQTT...");
    String clientId = "ESP8266Client-" + String(WiFi.macAddress());
    if (client.connect(clientId.c_str())) {
      Serial.println(" Connecté à MQTT");
      client.subscribe("test");
    } else {
      Serial.print(" Échec MQTT, rc=");
      Serial.print(client.state());
      Serial.println("Nouvelle tentative dans 2s...");
      delay(2000);
    }
  }
}

// ========== Publication heure ==========
void publishTime() {
  time_t epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  String weekDay = weekDays[timeClient.getDay()];
  struct tm* ptm = gmtime((time_t*)&epochTime);
  String currentDate = String(ptm->tm_year + 1900) + "-" + String(ptm->tm_mon + 1) + "-" + String(ptm->tm_mday);
  String fullTime = "Time: " + formattedTime + ", Date: " + currentDate + ", Day: " + weekDay;

  Serial.println(fullTime);
  client.publish("time", fullTime.c_str());
}

// ========== Page HTML ==========
void setupWebServer() {
  server.on("/", []() {
    server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Configuration WiFi ESP8266</title>
  <style>
    body { font-family: sans-serif; background: #f4f4f4; padding: 2em; text-align: center; }
    .card { background: #fff; padding: 2em; border-radius: 10px; max-width: 400px; margin: auto; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    input, button { width: 100%; padding: 10px; margin: 10px 0; border-radius: 5px; border: 1px solid #ccc; }
    button.connect { background-color: #28a745; color: white; font-weight: bold; }
    button.reset { background-color: #dc3545; color: white; font-weight: bold; }
    #feedback { margin-top: 1em; font-weight: bold; color: #333; }
  </style>
</head>
<body>
  <div class="card">
    <h2>Connexion Wi-Fi</h2>
    <input type="text" id="ssid" placeholder="Nom du réseau (SSID)">
    <input type="password" id="password" placeholder="Mot de passe">
    <button class="connect" onclick="sendWifi()">Se connecter</button>
    <button class="reset" onclick="resetESP()">Réinitialiser</button>
    <p id="feedback"></p>
  </div>
  <script>
    let socket = new WebSocket("ws://" + location.hostname + ":81");
    function sendWifi() {
      let ssid = document.getElementById("ssid").value;
      let password = document.getElementById("password").value;
      if (!ssid || !password) {
        document.getElementById("feedback").innerText = "Champs requis.";
        return;
      }
      socket.send(JSON.stringify({ ssid: ssid, password: password }));
      document.getElementById("feedback").innerText = "Connexion en cours...";
    }
    function resetESP() {
      socket.send(JSON.stringify({ commande: "reset" }));
      document.getElementById("feedback").innerText = "Réinitialisation...";
    }
  </script>
</body>
</html>
    )rawliteral");
  });
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  setupWebServer();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  String savedSSID = EEPROMReadString(SSID_ADDR, 32);
  String savedPASS = EEPROMReadString(PASS_ADDR, 64);

  if (savedSSID.length() > 0 && savedPASS.length() > 0) {
    connectToWiFi(savedSSID, savedPASS);
  } else {
    mode_config();
  }
}

// ========== Loop ==========
void loop() {
  webSocket.loop();
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnectMQTT();
    client.loop();

    static unsigned long lastTime = 0;
    if (millis() - lastTime >= 10000) {
      timeClient.update();
      publishTime();
      lastTime = millis();
    }
  }
}
