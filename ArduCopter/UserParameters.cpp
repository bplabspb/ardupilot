#include "UserParameters.h"

// "USR" + 13 chars remaining for param name 
const AP_Param::GroupInfo UserParameters::var_info[] = {
    
    // Put your parameters definition here
    // Note the maximum length of parameter name is 13 chars
    AP_GROUPINFO("USR_PI_ADDRESS", 0, UserParameters, _pi_address, 0x46),
    
    AP_GROUPEND
};
