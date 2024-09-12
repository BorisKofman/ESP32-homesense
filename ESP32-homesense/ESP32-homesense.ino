#include "HomeSpan.h"
#include "config.h"
#include <HardwareSerial.h>


#include <esp_bt.h>
#include <esp_bt_main.h>

// Ensure proper radar initialization based on the sensor type
#ifdef USE_LD2410
#include <ld2410.h>  
typedef ld2410 RadarType;
const int baudRate = 256000;
#endif

#ifdef USE_LD2450
#include "LD2450.h" 
typedef LD2450 RadarType;
const int baudRate = 256000;
#endif

#ifdef USE_LD2412
#include "LD2412.h"  
typedef LD2412 RadarType;
const int baudRate = 115200;  // Default baud rate for LD2412
#endif

#include "RadarAccessory.h" 
#include "VirtualSwitch.h"

// Initialize variables
unsigned long previousMillis = 0; 
const long interval = 5000;  // Interval for 5 seconds for printing 

// Declare hardware serial for radar communication
HardwareSerial radarSerial(1); 
RadarType radar; 

// Define variables for radarSerial setup
const int dataBits = SERIAL_8N1;


// Initialize radar with HardwareSerial reference
RadarType radar(radarSerial);


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

  #ifdef USE_LD2410
  radar.begin(radarSerial);
  if (radar.isInitialized()) {
    Serial.println("LD2410 radar sensor initialized successfully.");
  } else {
    Serial.println("Failed to initialize LD2410 radar sensor.");
    return; 
  }
  #endif

  #ifdef USE_LD2412
  radar.begin();  // No parameter needed
  Serial.println("LD2412 radar sensor initialized successfully.");
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

  unsigned long currentMillis = millis();

  // Check if 5 seconds have passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 

    Serial.println("Radar Data Report:");

    #if defined(USE_LD2410) || defined(USE_LD2412)
    if (radar.presenceDetected()) {
      Serial.println("Presence detected!");

      if (radar.stationaryTargetDetected()) {
        Serial.print("Stationary target detected at ");
        Serial.print(radar.stationaryTargetDistance());
        Serial.print(" cm");
      } else {
        Serial.println("No stationary target detected.");
      }

      if (radar.movingTargetDetected()) {
        Serial.print("Moving target detected at ");
        Serial.print(radar.movingTargetDistance());
        Serial.print(" cm");
      } else {
        Serial.println("No moving target detected.");
      }
    } else {
      Serial.println("No presence detected.");
    }
    #endif

    #ifdef USE_LD2450
    bool presenceDetected = false;

    // Iterate through all detected targets
    for (int i = 0; i < radar.getSensorSupportedTargetCount(); ++i) {
      auto target = radar.getTarget(i);
      if (target.valid) {
        presenceDetected = true;

        Serial.print("Detected Target ID: ");
        Serial.print(target.id);
        Serial.print(", Distance: ");
        Serial.print(target.distance);
        Serial.print(" cm, Speed: ");
        Serial.println(target.speed);
      }
    }

    if (!presenceDetected) {
      Serial.println("No presence detected.");
    }
    #endif

    Serial.println("-----------------------------");
  }
}