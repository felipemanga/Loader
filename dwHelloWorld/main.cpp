#include "../kernel/kapi.h"
#include "../desktop/api.h"

DESKTOP::API *desktop;
int c, pressed;

void setup( DESKTOP::API *dtapi ){
    desktop = dtapi;
}

void loop( KAPI *kernel ){
    if( isPressedLeft() || isPressedRight() ) c += pressed?0:++pressed;
    else pressed = 0;
    desktop->setCursor( 0, desktop->height-5 );
    desktop->setDrawColor(c);
    desktop->printf("%d", c);
}


