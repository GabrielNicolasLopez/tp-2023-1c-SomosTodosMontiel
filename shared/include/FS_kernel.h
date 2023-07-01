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
        FS_OK,
        FS_ERROR

    } t_FS_header;

    typedef struct
    {
        t_tipoInstruccion tipo; 
        uint32_t longitud_cadena;
        char *cadena;     
        uint32_t paramIntA;  // Puntero del archivo
        uint32_t paramIntB;  // Cant. Bytes
        uint32_t paramIntC;  // Dir. Memoria Física

    } t_instruccion_FS;

#endif