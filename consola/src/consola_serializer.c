#include "consola_serializer.h"

void consola_serializer_pack_no_args(t_buffer* buffer, t_tipoInstruccion instruccion) {
    buffer_pack(buffer, &instruccion, sizeof(t_instruccion));
}

void consola_serializer_pack_one_args(t_buffer* buffer, t_tipoInstruccion instruccion, uint32_t op1){
    consola_serializer_pack_no_args(buffer, instruccion);
    buffer_pack(buffer, &op1, sizeof(uint32_t));
}
