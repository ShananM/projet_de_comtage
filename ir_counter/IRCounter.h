#ifndef IRCOUNTER_H
#define IRCOUNTER_H

#include <Arduino.h>



class IRCounter {
public:
  IRCounter(uint8_t irPin, uint8_t ledPin, const char* label = "")
    : _irPin(irPin), _ledPin(ledPin), _label(label), passageCount(0),
      beamBroken(false), passageCounted(false), signalState(HIGH),
      lastChangeTime(0), signalHighStart(0) {}

  void begin() {
    pinMode(_irPin, INPUT_PULLUP);
    pinMode(_ledPin, OUTPUT);
    lastChangeTime = millis();
  }

  void update() {
    //unsigned long now = millis();
    signed long elapsed = millis() - lastChangeTime;
    
    /*
    if (signalState == HIGH) {
      signed long elapsed = now - lastChangeTime;
      if (elapsed > 100) {
        beamBroken = true;
        passageCounted = true;
      }
    }
    */
    if (signalState == HIGH && (elapsed > 100)) {
      beamBroken = true;
      signalHighStart = millis();
    }

    elapsed = millis() - signalHighStart;
    if (beamBroken && (elapsed > 400)) {
      passageCount++;
      beamBroken = false;
      passageCounted = true;
      Serial.print("Passage détecté [");
      Serial.print(_label);
      Serial.print("] Total : ");
      Serial.println(passageCount);
    }

    digitalWrite(_ledPin, digitalRead(_irPin) ? LOW : HIGH);
  }

  unsigned long getCount() const { return passageCount; }

  void handleIRChange() {
    signalState = digitalRead(_irPin);
    //Serial.print(signalState);
    lastChangeTime = millis();
  }

  static IRCounter* instance1;
  static IRCounter* instance2;

  static void IRAM_ATTR isr1() { if (instance1) instance1->handleIRChange(); }
  static void IRAM_ATTR isr2() { if (instance2) instance2->handleIRChange(); }

private:
  uint8_t _irPin, _ledPin;
  const char* _label;
  volatile bool signalState;
  volatile unsigned long lastChangeTime;
  unsigned long signalHighStart;
  bool beamBroken, passageCounted;
  unsigned long passageCount;
};

#endif
