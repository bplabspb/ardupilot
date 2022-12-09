#pragma once

#include <AP_HAL/I2CDevice.h>
#include <AP_HAL/AP_HAL.h>
//#include <AP_Param/AP_Param.h>
#include <GCS_MAVLink/GCS.h>

class BPLab_Pi_Interconnect
{
    public:
        void init(void);
        void send_heartbeat_message(void);
    private:
        bool _pi_found;
        int _pi_address;

        AP_HAL::OwnPtr<AP_HAL::I2CDevice> _dev;
};