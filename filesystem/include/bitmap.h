#ifndef BITMAP_H_
#define BITMAP_H_

    #include "filesystem.h"

    extern struct stat stats_fd_bitmap;
    extern char* p_bitmap;
    extern t_bitarray* bitA_bitmap;

    void crear_bitmap();
    void levantar_bitmap();

#endif