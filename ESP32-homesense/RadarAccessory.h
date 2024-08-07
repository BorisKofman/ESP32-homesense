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
    const uint32_t checkInterval = 5000;  // 5 seconds
    int minRange;  // Minimum detection range in cm
    int maxRange;  // Maximum detection range in cm

  public:
    RadarAccessory(ld2410 *radarSensor, int pin, int minRange, int maxRange) 
      : Service::OccupancySensor(),
        radar(radarSensor), 
        outPin(pin), minRange(minRange), maxRange(maxRange) {
      occupancy = new Characteristic::OccupancyDetected(0);
    }

    void loop() {
      uint32_t currentTime = millis();
      if (currentTime - lastCheckTime >= checkInterval) {
        lastCheckTime = currentTime;        
        // Check for presence detection based on combined range for stationary and moving targets
        bool presence = digitalRead(outPin) == HIGH || 
                        (radar->stationaryTargetDetected() && radar->stationaryTargetDistance() >= minRange && radar->stationaryTargetDistance() <= maxRange) || 
                        (radar->movingTargetDetected() && radar->movingTargetDistance() >= minRange && radar->movingTargetDistance() <= maxRange);

        // Update the occupancy characteristic for HomeSpan
        occupancy->setVal(presence ? 1 : 0);

        Serial.println("Radar Data Report:");

        if (radar->presenceDetected()) {
          Serial.println("Presence detected!");

          if (radar->stationaryTargetDetected()) {
            Serial.print("Stationary target detected at ");
            Serial.print(radar->stationaryTargetDistance());
            Serial.print(" cm, energy level: ");
            Serial.println(radar->stationaryTargetEnergy());
          } else {
            Serial.println("No stationary target detected.");
          }

          if (radar->movingTargetDetected()) {
            Serial.print("Moving target detected at ");
            Serial.print(radar->movingTargetDistance());
            Serial.print(" cm, energy level: ");
            Serial.println(radar->movingTargetEnergy());
          } else {
            Serial.println("No moving target detected.");
          }
        } else {
          Serial.println("No presence detected.");
        }

        Serial.println("-----------------------------");
      }
    }
};

#endif // RADAR_ACCESSORY_H