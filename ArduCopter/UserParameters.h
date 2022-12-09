#pragma once

#include <AP_Param/AP_Param.h>

class UserParameters {

public:
    UserParameters() {}
    static const struct AP_Param::GroupInfo var_info[];
    
    // usage example: g2.user_parameters.get_pi_address()
    AP_Int8 get_pi_address() const { return _pi_address; }
    
private:
    AP_PI_ADDRESS _pi_address;
};
