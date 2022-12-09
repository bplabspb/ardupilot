#pragma once

#include <AP_HAL/I2CDevice.h>
#include <Parameters.h>
#include <GCS_MAVLink/GCS.h>

public class BPLab_Pi_Interconnect
{
    public:
        void init(void);
        void send_heartbeat_message(void);
}