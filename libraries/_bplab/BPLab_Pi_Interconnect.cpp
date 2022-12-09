#include "BPLab_Pi_Interconnect.h"

void BPLab_Pi_Interconnect::init(void)
{
    //gcs().send_text(MAV_SEVERITY_CRITICAL, "BPLab production!");
}
void BPLab_Pi_Interconnect::send_heartbeat_message(void)
{
    gcs().send_text(MAV_SEVERITY_CRITICAL, "BPLab production! PI address is %i", g2.user_parameters.get_pi_address());
}