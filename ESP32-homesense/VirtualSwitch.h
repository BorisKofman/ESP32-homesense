#ifndef VIRTUAL_SWITCH_H
#define VIRTUAL_SWITCH_H

#include "HomeSpan.h"

class VirtualSwitch : public Service::Switch {
  public:
    SpanCharacteristic *power;  // Define a characteristic to hold the switch state

    VirtualSwitch() : Service::Switch() {
      power = new Characteristic::On(0);  // Initialize the switch state to OFF (0)
    }

    // This method is called whenever the switch state changes
    boolean update() override {
      if (power->getNewVal()) {
        Serial.println("Virtual Switch turned ON");
      } else {
        Serial.println("Virtual Switch turned OFF");
      }
      return true;  // Return true to indicate the update was successful
    }
};

#endif