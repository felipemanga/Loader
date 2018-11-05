inline bool getBacklight(){
    return (*(volatile uint32_t *)0xA0002108) & (1<<2);
}

inline void setBacklight( bool state ){
    if( state ) (*(volatile uint32_t *)0xA0002208) = 1<<2;
    else (*(volatile uint32_t *)0xA0002284) = 1<<2;
}

inline void fadeBacklight( bool state ){

    if( state == getBacklight() )
	return;
	
    volatile uint32_t &toggle = *(uint32_t *) 0xA0002308;

    toggle = 1<<2;
    uint32_t max = 0x400;
    for( uint32_t i=0; i<max; ++i ){

	for( volatile uint32_t j=0; j<i; j++ );
	toggle = 1<<2;

	for( volatile uint32_t j=i; j<max; j++ );
	toggle = 1<<2;
	    
    }

}
