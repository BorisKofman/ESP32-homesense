#include <Arduino.h>
#include "HomeSpan.h"
#include "RadarAccessory.h"  // Include the accessory class

void setup() {
  Serial.begin(115200);

  // Initialize HomeSpan
  homeSpan.begin(Category::Bridges, "HomeSpan Radar & BLE Bridge");
  homeSpan.setApTimeout(300);
  homeSpan.enableAutoStartAP();

  // Initialize HomeSpan accessories
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor");
    new RadarAccessory(4, 44, 43, 0, 300);  // Pass outPin, RX, TX pins, and detection range
}

void loop() {
  homeSpan.poll();  // HomeSpan processing
}