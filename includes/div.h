#pragma once

typedef struct {
    int quot;
    int rem;
} IDIV_RETURN_T;

typedef struct {
    unsigned quot;
    unsigned rem;
} UIDIV_RETURN_T;

typedef struct {
    int (*sidiv)(int numerator, int denominator); /*!< Signed integer division */
    unsigned (*uidiv)(unsigned numerator, unsigned denominator); /*!< Unsigned integer division */
    IDIV_RETURN_T (*sidivmod)(int numerator, int denominator); /*!< Signed integer division with remainder */
    UIDIV_RETURN_T (*uidivmod)(unsigned numerator, unsigned denominator); /*!< Unsigned integer division
									    with remainder */
} ROM_DIV_API_T;

typedef struct {
const uint32_t usbdApiBase; /*!< USBD API function table base address */
const uint32_t reserved0;
/*!< Reserved */
const uint32_t reserved1;
/*!< Reserved */
const void *pPWRD; /*!< Power API function table base address */
const ROM_DIV_API_T *divApiBase; /*!< Divider API function table base address */
} LPC_ROM_API_T;

#define LPC_ROM_API_BASE_LOC (0x1FFF1FF8UL)
#define LPC_ROM_API (*(LPC_ROM_API_T * *) LPC_ROM_API_BASE_LOC)


void miniitoa(unsigned long n, char *buf, uint8_t base=10 ){
    unsigned long i = 0;

    const ROM_DIV_API_T *pROMDiv = LPC_ROM_API->divApiBase;
    
    do{
	UIDIV_RETURN_T div = pROMDiv->uidivmod( n, base );
	char digit = div.rem;

	if( digit < 10 ) digit += '0';
	else digit += 'A';
	    
	buf[i++] = digit;
	// n /= base;
	n = div.quot;
	
    }while( n > 0 );

    buf[i--] = 0;

    for( unsigned long j=0; i>j; --i, ++j ){
	char tmp = buf[i];
	buf[i] = buf[j];
	buf[j] = tmp;
    }

}

// #define itoa(x, y) miniitoa(x, y)
#define itoa(x, y, z) miniitoa(x, y, z)
