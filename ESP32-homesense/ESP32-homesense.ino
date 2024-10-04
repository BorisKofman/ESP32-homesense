#include "HomeSpan.h"
#include "config.h"
#include <HardwareSerial.h>


#include <esp_bt.h>
#include <esp_bt_main.h>

#if defined(USE_LD2412)
#include "LD2412.h"  
typedef LD2412 RadarType;
const int baudRate = 115200;  // Default baud rate for LD2412
HardwareSerial radarSerial(1); 
RadarType radar(radarSerial);  // Pass radarSerial to the LD2412 constructor
const int dataBits = SERIAL_8N1;
#elif defined(USE_LD2410)
#include <ld2410.h>  
typedef ld2410 RadarType;
const int baudRate = 256000;
HardwareSerial radarSerial(1); 
RadarType radar;  // Radar for LD2410 (no constructor call)
const int dataBits = SERIAL_8N1;
#elif defined(USE_LD2450)
#include "LD2450.h" 
typedef LD2450 RadarType;
const int baudRate = 256000;
HardwareSerial radarSerial(1); 
RadarType radar; 
const int dataBits = SERIAL_8N1;
#endif

#include "RadarAccessory.h" 
#include "VirtualSwitch.h"

void setup() {
  Serial.begin(115200);

  // Disable Bluetooth to save power
  btStop();
  esp_bt_controller_disable();

  homeSpan.setStatusPixel(STATUS_LED_PIN, 240, 100, 5);
  homeSpan.setStatusAutoOff(5);
  homeSpan.begin(Category::Bridges, "HomeSense");
  homeSpan.enableWebLog(10, "pool.ntp.org", "UTC+3");
  homeSpan.setApTimeout(300);
  homeSpan.enableAutoStartAP();

  radarSerial.begin(baudRate, dataBits, rxPin, txPin);
  delay(500);

  #if defined(USE_LD2412) || defined(USE_LD2410)
  radar.begin(radarSerial);
  Serial.println("LD2410 radar sensor initialized successfully.");
  #endif

  #ifdef USE_LD2450
  radar.begin(radarSerial);
  Serial.println("LD2450 radar sensor initialized successfully.");
  #endif


  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();            

  // Add radar sensor 1 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 1");
    // Ensure RadarAccessory is defined or replace it with correct type
    new RadarAccessory(&radar, 0, 1100);  // Provide pin

  // // Add radar sensor 2 
  // new SpanAccessory();                                                          
  //   new Service::AccessoryInformation();
  //     new Characteristic::Identify(); 
  //     new Characteristic::Name("Radar Sensor 2");
  //   // Ensure RadarAccessory is defined or replace it with correct type
  //   new RadarAccessory(&radar, 350, 800);  // Provide pin

  // // Add a virtual switch
  // new SpanAccessory();
  //   new Service::AccessoryInformation();
  //     new Characteristic::Identify(); 
  //     new Characteristic::Name("Virtual Switch 1");
  //   // Ensure VirtualSwitch is defined or replace it with correct type
  //   new VirtualSwitch();  
}

void loop() {
  homeSpan.poll();
  radar.read(); 
}