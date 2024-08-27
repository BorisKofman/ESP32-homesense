#include "HomeSpan.h"
#include <HardwareSerial.h>

#define  USE_LD2450

#ifdef USE_LD2410
#include <ld2410.h>  // Include the ld2410 header file
typedef ld2410 RadarType; // Define RadarType as ld2410
#endif

#ifdef USE_LD2450
#include "LD2450.h" // Include the LD2450 header file
typedef LD2450 RadarType; // Define RadarType as LD2450
#endif

#include "RadarAccessory.h"  // Include the accessory class
#include "VirtualSwitch.h"   // Include the Virtual Switch class  

#define STATUS_LED_PIN 48  // pin for status LED

HardwareSerial radarSerial(1);  // Define radarSerial as Serial1
RadarType radar;  // Define radar object

unsigned long previousMillis = 0; 
const long interval = 5000;  // Interval for 5 seconds

// Define variables for radarSerial setup
const int baudRate = 256000;
const int dataBits = SERIAL_8N1;
const int rxPin = 44;
const int txPin = 43;
const int out = 4;

void setup() {
  Serial.begin(115200);

  // Initialize HomeSpan
  homeSpan.setStatusPixel(STATUS_LED_PIN, 240, 100, 5);
  homeSpan.begin(Category::Bridges, "HomeSense");
  homeSpan.enableWebLog(10, "pool.ntp.org", "UTC+3");
  homeSpan.setApTimeout(300);
  homeSpan.enableAutoStartAP();

  // Initialize the radar serial communication
  radarSerial.begin(baudRate, dataBits, rxPin, txPin);
  delay(500);

  #ifdef USE_LD2410
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
  #endif

  #ifdef USE_LD2450
  if (radar.begin(radarSerial)) {
    Serial.println("LD2450 radar sensor initialized successfully.");
  } else {
    Serial.println("Failed to initialize LD2450 radar sensor.");
    return;  // Stop if initialization fails
  }
  #endif

  // Add a radar sensor
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Radar Sensor 1");
    new RadarAccessory(&radar, out, 0, 1300);  // Pass radar object, outPin, and detection range

  // Add a virtual switch
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Virtual Switch 1");
    new VirtualSwitch();  // Create a virtual switch that will print to Serial
}

void loop() {
  homeSpan.poll();  // HomeSpan processing
  radar.read();  // Read radar data in the main loop
  
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
        Serial.print("Target count: ");
        Serial.println(radar.getSensorSupportedTargetCount());

        for (int i = 0; i < radar.getSensorSupportedTargetCount(); ++i) {
            auto target = radar.getTarget(i);
            if (target.valid) {
                Serial.print("Target ID: ");
                Serial.print(target.id);
                Serial.print(", Distance: ");
                Serial.print(target.distance);
                Serial.print(" cm, Speed: ");
                Serial.println(target.speed);
            } else {
                Serial.print("No valid target at index ");
                Serial.println(i);
            }
        }
        #endif

        Serial.println("-----------------------------");
    }
}