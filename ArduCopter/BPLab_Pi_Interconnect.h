#pragma once

#include <AP_HAL/I2CDevice.h>

#define BPLAB_PI_CMD_NOTHING 0x00

#define BPLAB_PI_CMD_HELLO 0x01
#define BPLAB_PI_CMD_HBT 0x02
#define BPLAB_PI_CMD_START_RECOGNITION 0x03
#define BPLAB_PI_CMD_STOP_RECOGNITION 0x04

#define BPLAB_PI_CMD_HELLO_OK 0xF1
#define BPLAB_PI_CMD_HBT_OK 0xF2
#define BPLAB_PI_CMD_START_RECOGNITION_OK 0xF3
#define BPLAB_PI_CMD_START_RECOGNITION_NOK 0xF4
#define BPLAB_PI_CMD_STOP_RECOGNITION_OK 0xF5
#define BPLAB_PI_CMD_STOP_RECOGNITION_NOK 0xF6

#define BPLAB_PI_MESSAGE_SIZE 0x01

#define BPLAB_GCS_DEBUG 1

class BPLab_Pi_Interconnect
{
    public:
        void init(int pi_address);
        bool start_recognition(void);
        bool stop_recognition(void);
    private:
        bool _pi_found;
        int _init_delay;
        int _hbt_delay;
        AP_HAL::OwnPtr<AP_HAL::I2CDevice> _dev;
        void timer(void);
        bool send_simple_cmd_without_lock(int cmd);
        bool read_without_lock();
        uint8_t _read_buffer[BPLAB_PI_MESSAGE_SIZE + 1];
        bool _need_to_send_start_recognition;
        bool _need_to_send_stop_recognition;
        
    // singleton
    public:
        static BPLab_Pi_Interconnect& getInstance()
        {
            static BPLab_Pi_Interconnect instance;
            return instance;
        }
        BPLab_Pi_Interconnect(BPLab_Pi_Interconnect const&) = delete;
        void operator=(BPLab_Pi_Interconnect const&) = delete;
    private:
        BPLab_Pi_Interconnect() {}
};