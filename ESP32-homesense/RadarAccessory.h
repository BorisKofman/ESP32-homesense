#ifndef RADAR_ACCESSORY_H
#define RADAR_ACCESSORY_H

#ifdef USE_LD2450
#include "LD2450.h"  // Include the LD2450 header file
#else
#include <ld2410.h>  // Include the LD2410 header file
#endif

#include <HardwareSerial.h>

class RadarAccessory : public Service::OccupancySensor {
  private:
    SpanCharacteristic *occupancy;
    
    #ifdef USE_LD2450
    LD2450 *radar;  // Pointer to the LD2450 radar object
    #else
    ld2410 *radar;  // Pointer to the LD2410 radar object
    #endif

    int outPin;
    uint32_t lastCheckTime = 0;
    const uint32_t checkInterval = 2000;  // 2 seconds
    int minRange;  // Minimum detection range in cm
    int maxRange;  // Maximum detection range in cm

  public:
    RadarAccessory(
      #ifdef USE_LD2450
      LD2450 *radarSensor, 
      #else
      ld2410 *radarSensor, 
      #endif
      int pin, int minRange, int maxRange) 
      : Service::OccupancySensor(),
        outPin(pin), minRange(minRange), maxRange(maxRange) {
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
              for (uint16_t i = 0; i < radar->getSensorSupportedTargetCount(); i++) {
                  LD2450::RadarTarget target = radar->getTarget(i);
                  if (target.valid) {
                      if ((target.distance >= minRange && target.distance <= maxRange)) {
                          presence = true;
                          break;
                      }
                  }
              }
          }
          #else
          if (radar->presenceDetected()) {
              // Check if either target is within the specified range
              if ((radar->stationaryTargetDistance() >= minRange && radar->stationaryTargetDistance() <= maxRange) || 
                  (radar->movingTargetDistance() >= minRange && radar->movingTargetDistance() <= maxRange)) {
                  presence = true;
              }
          }
          #endif

          // Update the occupancy characteristic for HomeSpan
          occupancy->setVal(presence);
      }
    }
};

#endif // RADAR_ACCESSORY_H