#include "HomeSpan.h"
#include "RadarAccessory.h"  // Include the accessory class
#include <HardwareSerial.h>
#include <ld2410.h>  // Include the ld2410 header file

HardwareSerial radarSerial(1);  // Define radarSerial as Serial1
ld2410 radar;  // Define radar object

void setup() {
  Serial.begin(115200);

  // Initialize HomeSpan
  homeSpan.begin(Category::Bridges, "HomeSpan Radar & BLE Bridge");
  homeSpan.setApTimeout(300);
  homeSpan.enableAutoStartAP();

  // Initialize the radar serial communication
  radarSerial.begin(256000, SERIAL_8N1, 43, 44);  // UART setup with RX and TX pins
  delay(500);
  if (radar.begin(radarSerial)) {
    Serial.println("LD2410 radar sensor initialized successfully.");
    Serial.print("LD2410 firmware version: ");
    Serial.print(radar.firmware_major_version);
    Serial.print('.');
    Serial.print(radar.firmware_minor_version);
    Serial.print('.');
    Serial.println(radar.firmware_bugfix_version, HEX);
  } else {
    Serial.println("Failed to initialize LD2410 radar sensor.");
    return;  // Stop if initialization fails
  }

  // Initialize HomeSpan accessories
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 1");
    new RadarAccessory(&radar, 4, 0, 200);  // Pass radar object, outPin, and detection range

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 2");
    new RadarAccessory(&radar, 4, 200, 900);  // Pass radar object, outPin, and detection range  ?>,mnbvcxzXcsdvfbg 
    "|"fghrtyedwsqa S1E23=-
}

void loop() {
  homeSpan.poll();  // HomeSpan processing
  radar.read();  // Read radar data in the main loop
}