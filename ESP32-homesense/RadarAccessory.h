#ifndef RADAR_ACCESSORY_H
#define RADAR_ACCESSORY_H

#include <ld2410.h>  // Include the ld2410 header file
#include <HardwareSerial.h>

class RadarAccessory : public Service::OccupancySensor {
  private:
    SpanCharacteristic *occupancy;
    ld2410 radar;  // Radar object instantiated within the class
    int outPin;
    int rxPin;  // Store RX pin
    int txPin;  // Store TX pin
    HardwareSerial radarSerial;  // Define radarSerial as Serial1
    uint32_t lastCheckTime = 0;
    const uint32_t checkInterval = 5000;  // 10 seconds
    int minRange;  // Minimum detection range in cm
    int maxRange;  // Maximum detection range in cm

  public:
    RadarAccessory(int pin, int rxPin, int txPin, int minRange, int maxRange) 
      : Service::OccupancySensor(),
        radarSerial(1),  // Initialize radarSerial as Serial1
        rxPin(rxPin), txPin(txPin), 
        outPin(pin), minRange(minRange), maxRange(maxRange) {
      occupancy = new Characteristic::OccupancyDetected(0);

      // Initialize the radar serial communication
      radarSerial.begin(256000, SERIAL_8N1, txPin, rxPin);  // UART setup with RX and TX pins
      Serial.print("RX Pin: ");
      Serial.println(rxPin);
      Serial.print("TX Pin: ");
      Serial.println(txPin);  
      Serial.print("Out Pin: ");
      Serial.println(outPin);  
      delay(500);
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
    }

    void loop() {
      radar.read();  // Make sure this is called before checking presence
      uint32_t currentTime = millis();
      if (currentTime - lastCheckTime >= checkInterval) {
        lastCheckTime = currentTime;        
        // Check for presence detection based on combined range for stationary and moving targets
        bool presence = digitalRead(outPin) == HIGH || 
                        (radar.stationaryTargetDetected() && radar.stationaryTargetDistance() >= minRange && radar.stationaryTargetDistance() <= maxRange) || 
                        (radar.movingTargetDetected() && radar.movingTargetDistance() >= minRange && radar.movingTargetDistance() <= maxRange);

        // Update the occupancy characteristic for HomeSpan
        occupancy->setVal(presence ? 1 : 0);

        Serial.println("Radar Data Report:");

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

        Serial.println("-----------------------------");
      }
    }
};

#endif // RADAR_ACCESSORY_H
