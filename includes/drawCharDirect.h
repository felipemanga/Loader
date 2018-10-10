
int drawChar(int x, int y, const unsigned char *bitmap, unsigned int index){
    uint8_t w = *bitmap;
    uint8_t h = *(bitmap + 1);
    uint8_t hbytes=0, xtra=1;
    if (h==8 || h==16) xtra=0; //don't add if exactly on byte limit
    hbytes=(h>>3)+xtra; //GLCD fonts are arranged w+1 times h/8 bytes
    //bitmap = bitmap + 3 + index * h * ((w>>3)+xtra); //add an offset to the pointer (fonts !)
    bitmap = bitmap + 4 + index * (w * hbytes + 1); //add an offset to the pointer (fonts !)
    //int8_t i, j, byteNum, bitNum, byteWidth = (w + 7) >> 3;
    int8_t i, j, numBytes;
    numBytes = *bitmap++; //first byte of char is the width in bytes
    // GLCD fonts are arranged LSB = topmost pixel of char, so its easy to just shift through the column
    uint16_t bitcolumn; //16 bits for 2x8 bit high characters

    for (i = 0; i < numBytes; i++) {
	bitcolumn = *bitmap++;
	if (hbytes == 2) bitcolumn |= (*bitmap++)<<8; // add second byte for 16 bit high fonts
	
	for (j = 0; j < h; j++) {
	    if (bitcolumn&0x1) {
		DIRECT::setPixel(x + i, y + j,drawcolor);
	    } 
	    bitcolumn>>=1;
	}
    }
    return numBytes; // for character stepping
}
