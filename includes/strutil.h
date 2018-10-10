
void getExtension( char *out, const char *src ){
    while( *src && *src++ != '.' );
    for( uint32_t i=0; i<3 && *src; ++i ){
	*out = *src++;
	if( *out >= 'a' && *out <= 'z' )
	    *out &= 0xDF;
	out++;
    }
    *out = 0;
}

char *concat( char *str, const char *src ){
    while(*str) str++;
    while(*src) *str++ = *src++;
    *str = 0;
    return str;
}

uint32_t hash( const char *str ){
    uint32_t h = 5381;
    while( *str )
	h = ((h << 5) + h) + *str++;
    return h;
}

size_t strlen( const char *str ){
    size_t len = 0;
    while( *str++ ) len++;
    return len;
}

bool iequals( const char *a, const char *b ){
    while( *a && *b ){
	char la = *a++, lb = *b++;
	if( la >= 'a' && la <= 'z' ) la &= 0xDF;
	if( lb >= 'a' && lb <= 'z' ) lb &= 0xDF;
	if( la != lb )
	    return false;
    }
    return *a == *b;
}
