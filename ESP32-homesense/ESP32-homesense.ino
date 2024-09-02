#include "HomeSpan.h"
#include <HardwareSerial.h>

#define USE_LD2450

#ifdef USE_LD2410
#include <ld2410.h>  
typedef ld2410 RadarType;
#endif

#ifdef USE_LD2450
#include "LD2450.h" 
typedef LD2450 RadarType; 
#endif

#include "RadarAccessory.h" 
#include "VirtualSwitch.h"

#define STATUS_LED_PIN 48  // Pin for status LED

HardwareSerial radarSerial(1); 
RadarType radar; 

unsigned long previousMillis = 0; 
const long interval = 5000;  // Interval for 5 seconds  only for printing 

// Define variables for radarSerial setup
const int baudRate = 256000;
const int dataBits = SERIAL_8N1;

const int rxPin = 43;
const int txPin = 44;

void setup() {
  Serial.begin(115200);

  homeSpan.setStatusPixel(STATUS_LED_PIN, 240, 100, 5);
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
    Serial.print("LD2410 firmware version: ");
    Serial.print(radar.firmware_major_version);
    Serial.print('.');
    Serial.print(radar.firmware_minor_version);
    Serial.print('.');
    Serial.println(radar.firmware_bugfix_version, HEX);
  } else {
    Serial.println("Failed to initialize LD2410 radar sensor.");
    return; 
  }
  #endif

  #ifdef USE_LD2450
  radar.begin(radarSerial);
  Serial.println("LD2450 radar sensor initialized successfully.");
  #endif

  // Example Add a radar sensor 1 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 1");
    new RadarAccessory(&radar, 0, 250); 

  // Example Add a radar sensor 2 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 2");
    new RadarAccessory(&radar, 250, 800); 

  // Example Add a virtual switch
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Virtual Switch 1");
    new VirtualSwitch();  
}

void loop() {
  homeSpan.poll();
  radar.read();  
  
  unsigned long currentMillis = millis();

  // Check if 5 seconds have passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 

    Serial.println("Radar Data Report:");

    #ifdef USE_LD2410
    if (radar.presenceDetected()) {
      Serial.println("Presence detected!");

      if (radar.stationaryTargetDetected()) {
        Serial.print("Stationary target detected at ");
        Serial.print(radar.stationaryTargetDistance());
        Serial.print(" cm, energy level: ");
        Serial.println(radar.stationaryTargetEnergy());
      } else {
        Serial.println("No stationary target detected.");
      }

      if (radar.movingTargetDetected()) {
        Serial.print("Moving target detected at ");
        Serial.print(radar.movingTargetDistance());
        Serial.print(" cm, energy level: ");
        Serial.println(radar.movingTargetEnergy());
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