#ifndef FS_MEMORIA_H_
#define FS_MEMORIA_H_

    #include <shared_utils.h>
    
    typedef enum
    {
        //Header FS -> MEMORIA
        FS_M_READ, // LADO MEMORIA: LEER - LADO FS: ESCRIBIR
        FS_M_WRITE, // LADO MEMORIA: ESCRIBIR - LADO FS: LEER
        //Header MEMORIA -> FS
        FS_M_READ_OK, // LADO MEMORIA: LEER - LADO FS: ESCRIBIR
        FS_M_WRITE_OK // LADO MEMORIA: ESCRIBIR - LADO FS: LEER

    } t_FS_MEMORIA;

#endif