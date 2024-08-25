#ifndef RADAR_ACCESSORY_H
#define RADAR_ACCESSORY_H

#include <ld2410.h>  // Include the ld2410 header file
#include <HardwareSerial.h>

class RadarAccessory : public Service::OccupancySensor {
  private:
    SpanCharacteristic *occupancy;
    ld2410 *radar;  // Pointer to the radar object
    int outPin;
    uint32_t lastCheckTime = 0;
    const uint32_t checkInterval = 2000;  // 2 seconds
    int minRange;  // Minimum detection range in cm
    int maxRange;  // Maximum detection range in cm

  public:
    RadarAccessory(ld2410 *radarSensor, int pin, int minRange, int maxRange) 
      : Service::OccupancySensor(),
        radar(radarSensor), 
        outPin(pin), minRange(minRange), maxRange(maxRange) {
      occupancy = new Characteristic::OccupancyDetected(0, true);
    }

  void loop() {
    uint32_t currentTime = millis();
    if (currentTime - lastCheckTime >= checkInterval) {
        lastCheckTime = currentTime;

        bool presence = false;

        // Check if presence is detected
        if (radar->presenceDetected()) {
            // Check if either target is within the specified range
            if ((radar->stationaryTargetDistance() >= minRange || radar->stationaryTargetDistance() <= maxRange) || 
                (radar->movingTargetDistance() >= minRange && radar->movingTargetDistance() <= maxRange)) {
                presence = true;
            }
        }

        // Update the occupancy characteristic for HomeSpan
        occupancy->setVal(presence);
    }
  }
};

#endif // RADAR_ACCESSORY_H