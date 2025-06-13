#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h> 
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 
//WIFI
const char* ssid = "NETGEAREA758B-g026";
const char* password = "boulerouge62219!";
//MQTT
const char* mqtt_server = "192.168.1.10";//Adresse IP du Broker Mqtt
const int mqttPort = 1883; //port utilisé par le Broker 
long tps=0;
ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  client.publish("test", "Hello from ESP8266");
}
void loop() {
  reconnect();
  client.loop(); 
  //On utilise pas un delay pour ne pas bloquer la réception de messages 

}
void setup_wifi(){
  //connexion au wifi
  WiFiMulti.addAP(ssid, password);
  while ( WiFiMulti.run() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("MAC : ");
  Serial.println(WiFi.macAddress());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}
void setup_mqtt(){
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);//Déclaration de la fonction de souscription
  reconnect();
}
//Callback doit être présent pour souscrire a un topic et de prévoir une action 
void callback(char* topic, byte *payload, unsigned int length) {
    client.publish("test","test1");
 }
void reconnect(){
  while (!client.connected()) {
    Serial.println("Connection au serveur MQTT ...");
    if (client.connect("ESP32Client")) {
      Serial.println("MQTT connecté");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
  client.subscribe("test");//souscription au topic led pour commander une led
}
//Fonction pour publier un float sur un topic 
