#include <SDFileSystem.h>
#include "FATFileHandle.h"
#include <stdlib.h>
#include "fsapi.h"

SDFileSystem *sdfs;
uint32_t init();
extern "C" void (* const g_pfnVectors[])(void);

// fopen
FILE * fs_fopen( const char *file, const char *mode ){
    int flags = O_RDWR;
    if( mode[0] != 'r' ){
	flags |= O_CREAT;
	if( mode[0] == 'w' )
	    flags |= O_TRUNC;
	else if( mode[0] == 'a' )
	    flags |= O_APPEND;
    }
    return (FILE *) sdfs->open( file, flags );
}

// fclose
int fs_fclose( FILE *fp ){
    if( fp )
	((FATFileHandle*) fp)->close();
    return 0;
}

// fread
unsigned int fs_fread (void *buffer, size_t a, size_t b, FILE *fp){
    unsigned int r = ((FATFileHandle *) fp)->read( buffer, a*b );
    if( r == ~0UL ) r = 0;
    return r;
}

// ftell
long fs_ftell (FILE *fp){
    return ((FATFileHandle *) fp)->lseek(0, SEEK_CUR);
}

// fseek
int fs_fseek (FILE *fp, long pos, int whence){
    return ((FATFileHandle *) fp)->lseek( pos, whence );
}
    
// fwrite
unsigned int fs_fwrite (const void *buffer, size_t a, size_t b, FILE *fp){
    return ((FATFileHandle *) fp)->write( buffer, a*b );
}

// openddir
DIR * fs_opendir( const char *name ){
    return (DirHandle *) sdfs->opendir( name );
}

// readdir
dirent * fs_readdir( DIR *d ){
    return ((DirHandle *) d)->readdir();
}

long fs_telldir( DIR *d ){
    return ((DirHandle *) d)->telldir();
}

void fs_seekdir( DIR *d, long off ){
    ((DirHandle *) d)->seekdir( off );
}

// closedir
int fs_closedir( DIR *d ){
    if( !d ) return 0;
    return ((DirHandle *)d)->closedir();
}

// feof
int fs_feof( FILE *fp ){
    return 0;
}

extern "C" __attribute__ ((section(".bootable")))
FSAPI const boot = {
    sprintf,

    fs_fopen,
    fs_fclose,
    fs_fread,
    fs_ftell,
    fs_fseek,
    fs_fwrite,
    fs_opendir,
    fs_readdir,
    fs_telldir,
    fs_seekdir,
    fs_closedir,
    fs_feof,    
    
    init,
    0xB007AB1E,
    0x10000000,
    g_pfnVectors
};
