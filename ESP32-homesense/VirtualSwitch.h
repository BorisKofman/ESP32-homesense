#ifndef VIRTUAL_SWITCH_H
#define VIRTUAL_SWITCH_H

#include "HomeSpan.h"

class VirtualSwitch : public Service::Switch {
  public:
    SpanCharacteristic *power;

    VirtualSwitch() : Service::Switch() {
      power = new Characteristic::On(0, true); 
    }

    boolean update() override {
      if (power->getNewVal()) {
        Serial.println("Virtual Switch turned ON");
      } else {
        Serial.println("Virtual Switch turned OFF");
      }
      return true;
    }
};

#endif