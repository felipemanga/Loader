#pragma once

#include <stdlib.h>
#include <cstdint>

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

#if !defined(__FILE_defined)
struct FILE;
#endif

#if !defined(DIR) && !defined(MBED_DIRHANDLE_H)
#define NAME_MAX 255
struct DIR;
struct dirent {
    char d_name[NAME_MAX+1];
    unsigned char d_type;
    long d_off;
};
#endif

#define DT_RDO  0x01    /* Read only */
#define DT_HID  0x02    /* Hidden */
#define DT_SYS  0x04    /* System */
#define DT_VOL  0x08    /* Volume label */
#define DT_LFN  0x0F    /* LFN entry */
#define DT_DIR  0x10    /* Directory */
#define DT_ARC  0x20    /* Archive */
#define DT_MASK 0x3F    /* Mask of defined bits */

struct FSAPI {
    int (* const sprintf)( char * str, const char * format, ... );
    FILE *(* const fopen)( const char *file, const char *mode );
    int (* const fclose)(FILE *);
    unsigned int (* const fread)(void *, size_t, size_t, FILE *);
    long (* const ftell)(FILE *);
    int (* const fseek)(FILE *, long, int);
    size_t (* const fwrite)(const void *, size_t, size_t, FILE *);
    DIR * (* const opendir)( const char * );
    dirent *(* const readdir)( DIR * );
    long (* const telldir)( DIR *d );
    void (* const seekdir)( DIR *d, long off );
    int (* const closedir)( DIR * );
    int (* const feof)( FILE * );
    uint32_t (* const init)( );
    
    const uint32_t magic, version;
    void (* const * vectors)();
    
};

#define FS (*(FSAPI *) (0x40000 - sizeof(FSAPI)))

class File {
    FILE *fp;
public:
    File( const char *name, const char *mode ){
	fp = FS.fopen( name, mode );
    }

    operator bool(){
	return fp != nullptr;
    }

    File & operator =( const int &pos ){
	FS.fseek( fp, pos, SEEK_SET );
	return *this;
    }

    File & operator +=( const int &offset ){
	FS.fseek( fp, offset, SEEK_CUR );
	return *this;
    }

    File & operator -=( const int &offset ){
	FS.fseek( fp, offset, SEEK_CUR );
	return *this;
    }

    void seek( uint32_t pos, int ref ){
	FS.fseek( fp, pos, ref );
    }

    ~File(){
	FS.fclose(fp);
    }

    uint32_t read( void *ptr, uint32_t count ){
	return FS.fread( (char *) ptr, 1, count, fp );
    }

    template< typename T, size_t S > uint32_t read( T (&data)[S] ){
	return read( data, sizeof(data) );
    }

    File & operator >> (int &i){
	FS.fread( (char *) &i, 4, 1, fp );
	return *this;
    }

    File & operator << (int i){
	FS.fwrite( (char *) &i, 4, 1, fp );
	return *this;
    }

    File & operator << (const char *str ){
	size_t len = 0;
	for( len=0; str[len]; ++len );
	FS.fwrite( str, 1, len, fp );
	return *this;
    }

};
