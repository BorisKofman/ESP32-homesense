#include "HomeSpan.h"
#include "config.h"
#include <HardwareSerial.h>

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

HardwareSerial radarSerial(1); 
RadarType radar(radarSerial);  // Pass the serial to LD2412 during initialization
unsigned long previousMillis = 0; 
const long interval = 5000;  // Interval for 5 seconds for printing 

// Define variables for radarSerial setup
const int dataBits = SERIAL_8N1;

const int rxPin = 44;
const int txPin = 43;

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
  } else {
    Serial.println("Failed to initialize LD2410 radar sensor.");
    return; 
  }
  #endif

  #ifdef USE_LD2450
  radar.begin(radarSerial);
  Serial.println("LD2450 radar sensor initialized successfully.");
  #endif
  
  // Add radar sensor 1 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 1");
    new RadarAccessory(&radar, 0, 350, 0);  // Provide pin

  // Add radar sensor 2 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 2");
    new RadarAccessory(&radar, 350, 800, 1);  // Provide pin

  // Add a virtual switch
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Virtual Switch 1");
    new VirtualSwitch();  
}

void loop() {
  homeSpan.poll();
  radar.readData();  // Use readData to fetch radar data from LD2412

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

    #ifdef USE_LD2412
    if (radar.presenceDetected()) {
      Serial.println("Presence detected by LD2412!");

      // Check for stationary targets
      if (radar.stationaryTargetDetected()) {
        Serial.print("Stationary target detected by LD2412 at ");
        Serial.print(radar.stationaryTargetDistance());
        Serial.print(" cm");
      } else {
        Serial.println("No stationary target detected by LD2412.");
      }

      // Check for moving targets
      if (radar.movingTargetDetected()) {
        Serial.print("Moving target detected by LD2412 at ");
        Serial.print(radar.movingTargetDistance());
        Serial.print(" cm");
      } else {
        Serial.println("No moving target detected by LD2412.");
      }

    } else {
      Serial.println("No presence detected by LD2412.");
    }
    #endif

    Serial.println("-----------------------------");
  }
}