#include "BPLab_Pi_Interconnect.h"
#include <AP_HAL/AP_HAL.h>
//#include <AP_Param/AP_Param.h>
#include <GCS_MAVLink/GCS.h>

extern const AP_HAL::HAL& hal;

void BPLab_Pi_Interconnect::init(int pi_address)
{
    if(_pi_found || _init_delay < 20) {
        _init_delay++;
        return;
    }
    #if BPLAB_GCS_DEBUG
    gcs().send_text(MAV_SEVERITY_INFO, "dbg init begin");
    #endif

    _init_delay = 0;

    FOREACH_I2C(i) {
        AP_HAL::OwnPtr<AP_HAL::I2CDevice> dev = hal.i2c_mgr->get_device(i, pi_address);
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init probing bus %li", i);
        #endif
        if (!dev) {
            continue;
        }

        dev->get_semaphore()->take_blocking();

        const uint8_t send[1] = {BPLAB_PI_CMD_HELLO};
        if (!dev->transfer(send, sizeof(send), nullptr, 0)) {
            #if BPLAB_GCS_DEBUG
            gcs().send_text(MAV_SEVERITY_INFO, "dbg init error send hello");
            #endif
            dev->get_semaphore()->give();
            continue;
        }
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init sent hello");
        #endif

        // wait for PI answer
        hal.scheduler->delay(20);
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init reading hello");
        #endif
        
        const size_t expected_reply_len = 1;
        uint8_t rx_bytes[expected_reply_len + 1];
        memset(rx_bytes, 0, sizeof(rx_bytes));
        if (!dev->transfer(nullptr, 0, rx_bytes, expected_reply_len)) {
            #if BPLAB_GCS_DEBUG
            gcs().send_text(MAV_SEVERITY_INFO, "dbg init error read hello");
            #endif
            dev->get_semaphore()->give();
            continue;
        }

        dev->get_semaphore()->give();
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init got hello answer: %u", (unsigned int)rx_bytes[0]);
        #endif

        if(rx_bytes[0]==BPLAB_PI_ANSWER_OK) {
            _pi_found = true;
            _dev = dev;
            gcs().send_text(MAV_SEVERITY_INFO, "dbg init finished, pi found at %i on bus %li", pi_address, i);
            return;
        }
    }
   
}

void BPLab_Pi_Interconnect::check_heartbeat(void)
{
    if(!_pi_found)
        return;
    if(_hbt_delay < 30) {
        _hbt_delay++;
        return;
    }
    _hbt_delay = 0;

    switch(send_and_wait_simple_cmd(BPLAB_PI_CMD_HBT)) {
        case BPLAB_PI_ANSWER_FATAL:
            gcs().send_text(MAV_SEVERITY_ERROR, "BPLab pi i2c error");
            break;
            case BPLAB_PI_ANSWER_OK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi heartbeat ok");
            break;
            default:
            gcs().send_text(MAV_SEVERITY_ERROR, "BPLab pi heartbeat lost");
            break;
    }
}

void BPLab_Pi_Interconnect::send_status_to_gcs(void)
{
    if(!_pi_found)
    {
        gcs().send_text(MAV_SEVERITY_ERROR, "BPLab pi not found");
    }
    else gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi found");
    //g2.user_parameters.get_pi_address());
}

int BPLab_Pi_Interconnect::send_and_wait_simple_cmd(int cmd)
{
        _dev->get_semaphore()->take_blocking();

        const uint8_t send[1] = {(uint8_t)cmd};
        if (!_dev->transfer(send, sizeof(send), nullptr, 0)) {
            #if BPLAB_GCS_DEBUG
            gcs().send_text(MAV_SEVERITY_INFO, "dbg send_and_wait_simple_cmd error send %i", cmd);
            #endif
            _dev->get_semaphore()->give();
            return BPLAB_PI_ANSWER_FATAL;
        }
        // wait for PI answer
        hal.scheduler->delay(20);
        
        uint8_t rx_bytes[2];
        memset(rx_bytes, 0, sizeof(rx_bytes));
        if (!_dev->transfer(nullptr, 0, rx_bytes, 1)) {
            #if BPLAB_GCS_DEBUG
            gcs().send_text(MAV_SEVERITY_INFO, "dbg end_and_wait_simple_cmd error read answer");
            #endif
            _dev->get_semaphore()->give();
            return BPLAB_PI_ANSWER_FATAL;
        }

        _dev->get_semaphore()->give();
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg end_and_wait_simple_cmd error got answer: %u", (unsigned int)rx_bytes[0]);
        #endif

        return rx_bytes[0];
}