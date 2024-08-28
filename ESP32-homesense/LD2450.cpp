#ifndef LD2450_cpp
#define LD2450_cpp

#include "LD2450.h"
#include <cmath> 

LD2450::LD2450()
{
}

LD2450::~LD2450()
{
}

void LD2450::begin(Stream &radarStream)
{
    radar_uart = &radarStream;
    last_target_data = "";
}

void LD2450::begin(HardwareSerial &radarStream, bool already_initialized)
{
    if (!already_initialized)
    {
        radarStream.begin(LD2450_SERIAL_SPEED);
    }
    radar_uart = &radarStream;
    last_target_data = "";
}

#ifdef ENABLE_SOFTWARESERIAL_SUPPORT
void LD2450::begin(SoftwareSerial &radarStream, bool already_initialized)
{
    if (!already_initialized)
    {
        radarStream.begin(LD2450_SERIAL_SPEED);
    }
    radar_uart = &radarStream;
    last_target_data = "";
}
#endif

void LD2450::setNumberOfTargets(uint16_t _numTargets)
{
    if (_numTargets > LD2450_MAX_SENSOR_TARGETS)
    {
        _numTargets = LD2450_MAX_SENSOR_TARGETS;
    }
    numTargets = _numTargets;
}

String LD2450::getLastTargetMessage()
{
    return last_target_data;
}

uint8_t LD2450::read()
{
    if (radar_uart == nullptr)
    {
        return -2;
    }
    if (radar_uart->available())
    {
        byte rec_buf[LD2450_SERIAL_BUFFER] = {0};
        int len = radar_uart->readBytes(rec_buf, sizeof(rec_buf));
        if (len > 0)
        {
            return ProcessSerialDataIntoRadarData(rec_buf, len);
        }
    }
    return -1;
}

uint16_t LD2450::getSensorSupportedTargetCount()
{
    return numTargets;
}

LD2450::RadarTarget LD2450::getTarget(uint16_t _target_id)
{
    if (_target_id >= LD2450_MAX_SENSOR_TARGETS)
    {
        RadarTarget tmp;
        tmp.valid = false;
        return tmp;
    }
    return radarTargets[_target_id];
}

uint8_t LD2450::ProcessSerialDataIntoRadarData(byte rec_buf[], int len)
{
    uint8_t refreshed_targets = 0;

    for (int i = 0; i < len; i++)
    {
        if (rec_buf[i] == 0xAA && rec_buf[i + 1] == 0xFF && rec_buf[i + 2] == 0x03 && rec_buf[i + 3] == 0x00 && rec_buf[i + 28] == 0x55 && rec_buf[i + 29] == 0xCC)
        {
            int index = i + 4;
            last_target_data = "";

            for (uint16_t targetCounter = 0; targetCounter < LD2450_MAX_SENSOR_TARGETS; targetCounter++)
            {
                if (index + 7 < len)
                {
                    RadarTarget target;
                    target.x = (int16_t)(rec_buf[index] | (rec_buf[index + 1] << 8));
                    target.y = (int16_t)(rec_buf[index + 2] | (rec_buf[index + 3] << 8));
                    target.speed = (int16_t)(rec_buf[index + 4] | (rec_buf[index + 5] << 8));
                    target.resolution = (uint16_t)(rec_buf[index + 6] | (rec_buf[index + 7] << 8));

                    if (rec_buf[index + 1] & 0x80)
                        target.x -= 0x8000;
                    else
                        target.x = -target.x;
                    if (rec_buf[index + 3] & 0x80)
                        target.y -= 0x8000;
                    else
                        target.y = -target.y;
                    if (rec_buf[index + 5] & 0x80)
                        target.speed -= 0x8000;
                    else
                        target.speed = -target.speed;

                    // CALCULATE DISTANCE in cm
                    target.distance = sqrt(pow(target.x, 2) + pow(target.y, 2)) / 10.0;

                    target.valid = (target.resolution != 0);

                    radarTargets[targetCounter] = target;
                    radarTargets[targetCounter].id = targetCounter + 1;

                    String status = (abs(target.speed) > 0) ? "Moving" : "Stationary";

                    last_target_data += "TARGET ID=" + String(targetCounter + 1);
                    last_target_data += ", SPEED=" + String(target.speed) + " cm/s";
                    last_target_data += ", DISTANCE=" + String(target.distance, 1) + " cm";
                    last_target_data += ", STATUS=" + status;
                    last_target_data += ", VALID=" + String(target.valid) + "\n";

                    index += 8;

                    refreshed_targets++;

                    if (refreshed_targets >= numTargets)
                    {
                        break;
                    }
                }
                else
                {
                    radarTargets[targetCounter].valid = false;
                }
            }
            i = index;
        }
    }

    if (refreshed_targets == 0) {
        last_target_data = "No targets detected.\n";
    }

    return refreshed_targets;
}

bool LD2450::presenceDetected() {
    for (uint16_t i = 0; i < getSensorSupportedTargetCount(); i++) {
        RadarTarget target = getTarget(i);
        if (target.valid) {
            return true;
        }
    }
    return false;  
}
#endif