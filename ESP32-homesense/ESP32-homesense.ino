#include "HomeSpan.h"
#include <HardwareSerial.h>
#include <ld2410.h>  // Include the ld2410 header file
#include <WebServer.h>

#include "RadarAccessory.h"  // Include the accessory class
#include "VirtualSwitch.h" //Include the Virtual Switch class  


HardwareSerial radarSerial(1);  // Define radarSerial as Serial1
ld2410 radar;  // Define radar object

unsigned long previousMillis = 0; 
const long interval = 5000;  // Interval for 5 seconds

String sensorData = "No data available";

// Define variables for radarSerial setup
const int baudRate = 256000;
const int dataBits = SERIAL_8N1;
const int rxPin = 44;
const int txPin = 43;
const int out = 4;


void setup() {
  Serial.begin(115200);

  // Initialize HomeSpan
  homeSpan.begin(Category::Bridges, "HomeSense");
  homeSpan.setApTimeout(300);
  homeSpan.enableAutoStartAP();

  // Initialize web server
  server.on("/sensor", handleSensorData);  // Only handle /sensor route
  server.begin();
  Serial.println("Web server started on port 8080");

  // Initialize the radar serial communication
  radarSerial.begin(baudRate, dataBits, rxPin, txPin);
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

  // Add a reader senor
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
}

void loop() {
  homeSpan.poll();  // HomeSpan processing
  radar.read();  // Read radar data in the main loop

  server.handleClient();  // Handle web server requests

  unsigned long currentMillis = millis();

  // Check if 5 seconds have passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 

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
    sensorData = "Radar Data Report:\n\nMoving Target:\n" + movingTargetData + "\n\nStationary Target:\n" + stationaryTargetData;
    Serial.println("-----------------------------");
  }
}

void handleSensorData() {
  server.send(200, "text/plain", sensorData);  // Serve the sensor data
}