#ifndef CONSOLA_SERIALIZER_H_INCLUDED
#define CONSOLA_SERIALIZER_H_INCLUDED

#include "buffer.h"

void consola_serializer_pack_no_args(t_buffer*, t_tipoInstruccion instruccion);
void consola_serializer_pack_one_args(t_buffer* buffer, t_tipoInstruccion instruccion, uint32_t op1);

#endif
