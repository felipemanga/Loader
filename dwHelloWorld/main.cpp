#include "../kernel/kapi.h"
#include "api.h"

namespace DT {
    #include "../desktop/api.h"
}

DT::API *desktop;

void setup( DT::API *dtapi ){
    // desktop = dtapi;
    DBG(dtapi);
    // desktop->palette[0] = 0xFFFF;
    // desktop->lcdRefresh();
}

void loop( KAPI *kernel ){
    DBG(kernel);
}

API api = { loop };

