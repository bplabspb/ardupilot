#pragma once

#include <AP_HAL/I2CDevice.h>

#define BPLAB_PI_CMD_HELLO 0x01
#define BPLAB_PI_CMD_HBT 0x02

#define BPLAB_PI_ANSWER_OK 0x03
#define BPLAB_PI_ANSWER_NOK 0x06
#define BPLAB_PI_ANSWER_FATAL 0x06

#define BPLAB_GCS_DEBUG 1

class BPLab_Pi_Interconnect
{
    public:
        void init(int pi_address);
        void send_status_to_gcs(void);
        void check_heartbeat(void);
    private:
        bool _pi_found;
        int _init_delay;
        int _hbt_delay;
        AP_HAL::OwnPtr<AP_HAL::I2CDevice> _dev;

        int send_and_wait_simple_cmd(int cmd);
};