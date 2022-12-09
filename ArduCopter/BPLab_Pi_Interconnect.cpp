#include "BPLab_Pi_Interconnect.h"

extern const AP_HAL::HAL& hal;

void BPLab_Pi_Interconnect::init(void)
{

}
void BPLab_Pi_Interconnect::send_heartbeat_message(void)
{
    if(!_pi_found)
    {
        gcs().send_text(MAV_SEVERITY_ERROR, "BPLab pi not found at %i", _pi_address);
    }
    else gcs().send_text(MAV_SEVERITY_INFO, "BPLab pi found at %i", _pi_address);
    //g2.user_parameters.get_pi_address());
}