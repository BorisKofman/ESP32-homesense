#ifndef RADAR_ACCESSORY_H
#define RADAR_ACCESSORY_H

#ifdef USE_LD2450
#include "LD2450.h" 
#else
#include <ld2410.h>
#endif

#include <HardwareSerial.h>

class RadarAccessory : public Service::OccupancySensor {
  private:
    SpanCharacteristic *occupancy;
    
    #ifdef USE_LD2450
    LD2450 *radar;  
    #else
    ld2410 *radar;
    #endif

    uint32_t lastCheckTime = 0;
    const uint32_t checkInterval = 2000;  // 2 seconds
    int minRange;
    int maxRange;

  public:
    RadarAccessory(
      #ifdef USE_LD2450
      LD2450 *radarSensor, 
      #else
      ld2410 *radarSensor, 
      #endif
      int minRange, int maxRange) 
      : Service::OccupancySensor(), 
        minRange(minRange), maxRange(maxRange) {
      #ifdef USE_LD2450
      radar = radarSensor;
      #else
      radar = radarSensor;
      #endif
      occupancy = new Characteristic::OccupancyDetected(0, true);
    }

    void loop() {
      uint32_t currentTime = millis();
      if (currentTime - lastCheckTime >= checkInterval) {
          lastCheckTime = currentTime;

          bool presence = false;

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

          #else
          if (radar->presenceDetected()) {
              if ((radar->stationaryTargetDistance() >= minRange && radar->stationaryTargetDistance() <= maxRange) || 
                  (radar->movingTargetDistance() >= minRange && radar->movingTargetDistance() <= maxRange)) {
                  presence = true;
              }
          }
          #endif

          occupancy->setVal(presence);
      }
    }
};

#endif