#ifndef FS_KERNEL_H_
#define FS_KERNEL_H_

    #include <shared_utils.h>
    
    typedef enum
    {
        //Header kernel -> FS
        FS_INSTRUCCION,
        FS_FINALIZAR,
        //Header FS -> kernel
        FS_OPEN_OK,
        FS_OPEN_NO_OK,
        FS_CREATE_OK,

    } t_FS_header;

#endif