#ifndef LD2450_h
#define LD2450_h

#include <Arduino.h>

#ifdef SoftwareSerial_h
#define ENABLE_SOFTWARESERIAL_SUPPORT
#endif

#ifdef ENABLE_SOFTWARESERIAL_SUPPORT
#include <SoftwareSerial.h>
#endif

#define LD2450_MAX_SENSOR_TARGETS 3
#define LD2450_SERIAL_BUFFER 256
#define LD2450_SERIAL_SPEED 256000

class LD2450
{
public:
    typedef struct RadarTarget
    {
        uint16_t id;         // ID
        int16_t x;           // X mm
        int16_t y;           // Y mm
        int16_t speed;       // cm/s
        uint16_t resolution; // mm
        uint16_t distance;   // cm calculated from x and y
        bool valid;
    } RadarTarget;

    LD2450();
    ~LD2450();

    void begin(Stream &radarStream);
    void begin(HardwareSerial &radarStream, bool already_initialized = false);
    bool presenceDetected(); 

#ifdef ENABLE_SOFTWARESERIAL_SUPPORT
    void begin(SoftwareSerial &radarStream, bool already_initialized = false);
#endif

    void setNumberOfTargets(uint16_t _numTargets);
    uint8_t ProcessSerialDataIntoRadarData(byte rec_buf[], int len);
    RadarTarget getTarget(uint16_t _target_id);
    uint16_t getSensorSupportedTargetCount();
    String getLastTargetMessage();
    uint8_t read();

protected:

private:
    Stream *radar_uart = nullptr;
    RadarTarget radarTargets[LD2450_MAX_SENSOR_TARGETS];
    uint16_t numTargets = LD2450_MAX_SENSOR_TARGETS;
    String last_target_data = "";
};

#endif