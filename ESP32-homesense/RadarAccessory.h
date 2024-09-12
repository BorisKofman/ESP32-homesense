#ifndef RADAR_ACCESSORY_H
#define RADAR_ACCESSORY_H

#ifdef USE_LD2450
#include "LD2450.h" 
#elif defined(USE_LD2412)
#include "LD2412.h"
#else
#include <ld2410.h>
#endif

#include <HardwareSerial.h>

class RadarAccessory : public Service::OccupancySensor {
  private:
    SpanCharacteristic *occupancy;
    
    #ifdef USE_LD2450
    LD2450 *radar;  
    #elif defined(USE_LD2412)
    LD2412 *radar;
    #else
    ld2410 *radar;
    #endif

    int minRange;
    int maxRange;
    // Initialize variables
    unsigned long previousMillis = 0; 
    const long interval = 1000; 

  public:
    RadarAccessory(
      #ifdef USE_LD2450
      LD2450 *radarSensor, 
      #elif defined(USE_LD2412)
      LD2412 *radarSensor, 
      #else
      ld2410 *radarSensor, 
      #endif
      int minRange, int maxRange) 
      : Service::OccupancySensor(), 
        minRange(minRange), maxRange(maxRange) {
      #ifdef USE_LD2450
      radar = radarSensor;
      #elif defined(USE_LD2412)
      radar = radarSensor;
      #else
      radar = radarSensor;
      #endif
      occupancy = new Characteristic::OccupancyDetected(0, true);
    }

void loop() {
    bool presence = false;

    unsigned long currentMillis = millis();

    // Check if 5 seconds have passed
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      
#ifdef USE_LD2450
    if (radar->read() > 0) {
        bool anyTargetsDetected = false;

        for (uint16_t i = 0; i < radar->getSensorSupportedTargetCount(); i++) {
            LD2450::RadarTarget target = radar->getTarget(i);
            if (target.valid) {
                anyTargetsDetected = true;

                if (target.distance >= minRange && target.distance <= maxRange) {
                    presence = true;
                    break;
                }
            }
        }

        if (!anyTargetsDetected) {
            Serial.println("No targets detected.");
        }
    }
#elif defined(USE_LD2410) || defined(USE_LD2412)
    if (radar->presenceDetected()) {

      #ifdef DEBUG
        Serial.println("Presence detected.");
      #endif

        int stationaryDist = radar->stationaryTargetDistance();
        int movingDist = radar->movingTargetDistance();

      #ifdef DEBUG
        // Print the stationary and moving target distances
        Serial.print("Stationary target distance: ");
        Serial.println(stationaryDist);

        Serial.print("Moving target distance: ");
        Serial.println(movingDist);
      #endif

        if ((stationaryDist >= minRange && stationaryDist <= maxRange) || 
            (movingDist >= minRange && movingDist <= maxRange)) {
            presence = true;
          #ifdef DEBUG
            Serial.println("Presence within range.");
          #endif
        } else {
          #ifdef DEBUG
            Serial.println("Presence detected but out of range.");
          #endif
        }
    } else {
      #ifdef DEBUG
        Serial.println("No presence detected.");
      #endif
    }
#endif
      occupancy->setVal(presence);
    }
  }
};

#endif