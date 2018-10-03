#include "../kernel/kapi.h"
#include "../desktop/api.h"

void setup( DESKTOP::API *desktop ){
    
    desktop->itemStrideX = 0;
    desktop->itemStrideY = 42;
    desktop->itemOffsetX = desktop->width - 38;
    desktop->itemOffsetY = -8;
    desktop->moveX = 0;
    desktop->moveY = -40;
    
    desktop->lblX = desktop->width - 20;
    desktop->lblY = 28;
    desktop->lblColor = 0;

    desktop->clearColor = 1;
    desktop->clearX = desktop->width - 40;
    desktop->clearWidth = 40;
    
    FS.init("");
    
    if( FILE *f = FS.fopen("loader/wallpaper.16c", "r") ){
	
	FS.fread( desktop->screenbuffer, 1, 55*88, f );
	FS.fclose(f);
	
    }

}

