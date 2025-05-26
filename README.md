(petit cate)
 
 //code coté émetteur
#include <ESP8266WiFi.h>

#define PIN_GPIO4 4  // Définir la GPIO4 comme sortie pour le signal
void start38khz(){
  analogWriteFreq(38000);
  analogWrite(PIN_GPIO4,77);
 }

void stop38khz(){
  analogWrite(PIN_GPIO4,0);
 }
 
void setup() {
  // Initialisation de la GPIO4 comme sortie
  pinMode(PIN_GPIO4, OUTPUT);
  // Aucun besoin de connexion Wi-Fi pour ce projet, donc aucune initialisation nécessaire
}

void loop() {
  // Générer un signal à 38 kHz sur la GPIO4
  start38khz();
  delay(1);
  stop38khz();
  delay(1);
 
  // Le signal continue indéfiniment. Vous pouvez ajuster le délai si nécessaire.
  //delay(10); // Délai de 10 ms, à ajuster si nécessaire
 
}

