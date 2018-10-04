#pragma once

#include <stdlib.h>
#include <cstdint>

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
    uint32_t (* const init)( const char * );
    
    const uint32_t magic, version;
    void (* const * vectors)();
    
};

#define FS (*(FSAPI *) (0x40000 - sizeof(FSAPI)))
