#pragma once
#include "vpx4.h"

//The custom instruction addition system is useful for device specific HALT
//SLEEP, IO and kernel implementations.

//NOTE: for extended isa, please name the function vpx4_isa_e_----

//[[ BASIC CONTROL INSTRUCTIONS ]]

void vpx4_isa_except(Vpx4* vpx4);
void vpx4_isa_nop(Vpx4* vpx4);
void vpx4_isa_hostcall(Vpx4* vpx4);


//[[ VPX3 COMPATIBLE ]]

void vpx4_isa_mov(Vpx4* vpx4);
void vpx4_isa_movi(Vpx4* vpx4);

void vpx4_isa_inc(Vpx4* vpx4);
void vpx4_isa_dec(Vpx4* vpx4);

void vpx4_isa_or(Vpx4* vpx4);
void vpx4_isa_xor(Vpx4* vpx4);
void vpx4_isa_and(Vpx4* vpx4);
void vpx4_isa_not(Vpx4* vpx4);

void vpx4_isa_ori(Vpx4* vpx4);
void vpx4_isa_xori(Vpx4* vpx4);
void vpx4_isa_andi(Vpx4* vpx4);

void vpx4_isa_sll(Vpx4* vpx4);
void vpx4_isa_srl(Vpx4* vpx4);
void vpx4_isa_sra(Vpx4* vpx4);

void vpx4_isa_slli(Vpx4* vpx4);
void vpx4_isa_srli(Vpx4* vpx4);
void vpx4_isa_srai(Vpx4* vpx4);

void vpx4_isa_add(Vpx4* vpx4);
void vpx4_isa_sub(Vpx4* vpx4);
void vpx4_isa_mul(Vpx4* vpx4);
void vpx4_isa_udiv(Vpx4* vpx4);
void vpx4_isa_sdiv(Vpx4* vpx4);
void vpx4_isa_urem(Vpx4* vpx4);
void vpx4_isa_srem(Vpx4* vpx4);

void vpx4_isa_addi(Vpx4* vpx4);
void vpx4_isa_subi(Vpx4* vpx4);
void vpx4_isa_muli(Vpx4* vpx4);
void vpx4_isa_udivi(Vpx4* vpx4);
void vpx4_isa_sdivi(Vpx4* vpx4);
void vpx4_isa_uremi(Vpx4* vpx4);
void vpx4_isa_sremi(Vpx4* vpx4);

void vpx4_isa_ld8i(Vpx4* vpx4);
void vpx4_isa_ld16i(Vpx4* vpx4);
void vpx4_isa_ld32i(Vpx4* vpx4);
void vpx4_isa_ld8r(Vpx4* vpx4);
void vpx4_isa_ld16r(Vpx4* vpx4);
void vpx4_isa_ld32r(Vpx4* vpx4);

void vpx4_isa_st8i(Vpx4* vpx4);
void vpx4_isa_st16i(Vpx4* vpx4);
void vpx4_isa_st32i(Vpx4* vpx4);
void vpx4_isa_st8r(Vpx4* vpx4);
void vpx4_isa_st16r(Vpx4* vpx4);
void vpx4_isa_st32r(Vpx4* vpx4);

void vpx4_isa_jmpi(Vpx4* vpx4);
void vpx4_isa_jmpr(Vpx4* vpx4);

void vpx4_isa_zjmpi(Vpx4* vpx4);
void vpx4_isa_eqjmpi(Vpx4* vpx4);
void vpx4_isa_nejmpi(Vpx4* vpx4);
void vpx4_isa_gjmpi(Vpx4* vpx4);
void vpx4_isa_gejmpi(Vpx4* vpx4);

void vpx4_isa_push8(Vpx4* vpx4);
void vpx4_isa_push16(Vpx4* vpx4);
void vpx4_isa_push32(Vpx4* vpx4);

void vpx4_isa_pop8(Vpx4* vpx4);
void vpx4_isa_pop16(Vpx4* vpx4);
void vpx4_isa_pop32(Vpx4* vpx4);

void vpx4_isa_calli(Vpx4* vpx4);
void vpx4_isa_callr(Vpx4* vpx4);

void vpx4_isa_ret(Vpx4* vpx4);
