#include "BPLab_Pi_Interconnect.h"
#include <AP_HAL/AP_HAL.h>
//#include <AP_Param/AP_Param.h>
#include <GCS_MAVLink/GCS.h>

extern const AP_HAL::HAL& hal;

void BPLab_Pi_Interconnect::init(int pi_address)
{
    if(_pi_found)
        return;
    
    if(_init_delay < 10) {
        _init_delay++;
        return;
    }
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

        // wait for PI answer
        hal.scheduler->delay(20);
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init reading hello");
        #endif
        
        memset(_read_buffer, 0, sizeof(_read_buffer));
        if (!dev->transfer(nullptr, 0, _read_buffer, BPLAB_PI_MESSAGE_SIZE)) {
            #if BPLAB_GCS_DEBUG
            gcs().send_text(MAV_SEVERITY_INFO, "dbg init error read hello");
            #endif
            dev->get_semaphore()->give();
            continue;
        }

        dev->get_semaphore()->give();
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init got hello answer: %u", (unsigned int)_read_buffer[0]);
        #endif

        if(_read_buffer[0] == BPLAB_PI_CMD_HELLO_OK) {
            _dev = std::move(dev);
            //10Hz
            _dev->register_periodic_callback(100000, FUNCTOR_BIND_MEMBER(&BPLab_Pi_Interconnect::timer, void));
            gcs().send_text(MAV_SEVERITY_INFO, "dbg init finished, pi found at %i on bus %li", pi_address, i);
            _pi_found = true;
            return;
        }    
    }
}

bool BPLab_Pi_Interconnect::stop_recognition(void)
{
    if(!_pi_found)
        return false;
    _need_to_send_stop_recognition = true;
    return true;
}

bool BPLab_Pi_Interconnect::start_recognition(void)
{
    if(!_pi_found)
        return false;
    _need_to_send_start_recognition = true;
    return true;
}

void BPLab_Pi_Interconnect::timer(void)
{
    // get data from pi 10 times a second
    if(!read_without_lock()) {
        return;
    }
    
    switch (_read_buffer[0])
    {
        case BPLAB_PI_CMD_START_RECOGNITION_OK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi got start recognition ok");
            break;
        case BPLAB_PI_CMD_START_RECOGNITION_NOK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi got start recognition error");
            break;
        case BPLAB_PI_CMD_STOP_RECOGNITION_OK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi got stop recognition ok");
            break;
        case BPLAB_PI_CMD_STOP_RECOGNITION_NOK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi got stop recognition error");
            break;
        case BPLAB_PI_CMD_HBT_OK:
            gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi got heartbeat");
            break;
        case BPLAB_PI_CMD_NOTHING:
            break;
    }

    // check if we need to send any command
    
    if(_need_to_send_stop_recognition){
        _need_to_send_stop_recognition = false;
        send_simple_cmd_without_lock(BPLAB_PI_CMD_STOP_RECOGNITION);
        gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi stop recognition send");
        return;
    }

    if(_need_to_send_start_recognition){
        _need_to_send_start_recognition = false;
        send_simple_cmd_without_lock(BPLAB_PI_CMD_START_RECOGNITION);
        gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi start recognition send");
        return;
    }

    if(_hbt_delay >= 600) {
        _hbt_delay = 0;
        send_simple_cmd_without_lock(BPLAB_PI_CMD_HBT);
        gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi heartbeat send");
    }
    else _hbt_delay++;    
}

bool BPLab_Pi_Interconnect::send_simple_cmd_without_lock(int cmd)
{
    const uint8_t send[1] = {(uint8_t)cmd};
    if (!_dev->transfer(send, sizeof(send), nullptr, 0)) {
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_ERROR, "dbg send_simple_cmd_without_lock error send %i", cmd);
        #endif
        return false;
    }
    return true;
}

bool BPLab_Pi_Interconnect::read_without_lock()
{
    memset(_read_buffer, 0, sizeof(_read_buffer));
    if (!_dev->transfer(nullptr, 0, _read_buffer, BPLAB_PI_MESSAGE_SIZE)) {
        #if BPLAB_GCS_DEBUG
        gcs().send_text(MAV_SEVERITY_INFO, "dbg init error read hello");
        #endif
        return false;
    }
    return true;
}