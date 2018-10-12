namespace DIRECT {

    uint32_t * const MPIN2 = (uint32_t *) 0xA0002188;
    
    uint32_t * const CLR_CD = (uint32_t *) 0xA0002280;
    uint32_t * const SET_CD = (uint32_t *) 0xA0002200;
    const uint32_t CD_PIN = 1<<2;
    
    uint32_t * const CLR_WR = (uint32_t *) 0xA0002284;
    uint32_t * const SET_WR = (uint32_t *) 0xA0002204;
    const uint32_t WR_PIN = 1<<12;
    
    inline void setup_data(uint16_t data)
    {
	*MPIN2 = uint32_t(data)<<3; // write bits to port
    }


/**************************************************************************/
/*!
  @brief  Write a command to the lcd, 16-bit bus
*/
/**************************************************************************/
    inline void write_command(uint16_t data)
    {
	*CLR_CD = CD_PIN; // CLR_CD; // clear CD = command
	setup_data(data); // function that inputs the data into the relevant bus lines
	
	// CLR_WR_SLOW;  // WR low
	*CLR_WR = WR_PIN;
	__asm("nop");
	
	// SET_WR;  // WR low, then high = write strobe
	*SET_WR = WR_PIN;
    }

/**************************************************************************/
/*!
  @brief  Write data to the lcd, 16-bit bus
*/
/**************************************************************************/
    inline void write_data(uint16_t data)
    {
	*SET_CD = CD_PIN;
	setup_data(data);
	
	*CLR_WR = WR_PIN;
	__asm("nop");
	
	*SET_WR = WR_PIN;
    }

    void setPixel( uint32_t x, uint32_t y, uint32_t color ){
	if( x>=220 || y>=176 ) return;
	write_command(0x20);
	write_data(y);
	write_command(0x21);
	write_data(x);
	write_command(0x22);

	*SET_CD = CD_PIN;
	setup_data( palette[color] );
	
	*CLR_WR = WR_PIN;
	__asm("nop");
	*SET_WR = WR_PIN;
    }

}
