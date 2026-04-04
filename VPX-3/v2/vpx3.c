//[[ INCLUDES ]]
#include <stdint.h>
#include <string.h>
#include "vpx3.h"

//[[ STRUCT DECLARE ]]
typedef struct Vpx3 Vpx3;

//[[ INLINE STATIC FUNCTIONS ]]


//[[ ERROR LOG ]]
void vpx3_log_err(Vpx3* vpx3, uint8_t code, uint32_t value){
    vpx3->err_code = code;
    vpx3->err_val = value;
    vpx3->err_pc_state = vpx3->registers[VPX_RPC];
}

//[[ REGISTER ]]

uint32_t vpx3_rreg(Vpx3* vpx3, uint8_t reg){
    //You may remove this check if you provide a different
    //register stack without using unsafe mode
    if(reg >= 64){
        //Log attempted register read
        vpx3_log_err(vpx3, 1, reg);
        return 0;
    }
    return vpx3->registers[reg];
}
void vpx3_wreg(Vpx3* vpx3, uint8_t reg, uint32_t val){
    if(reg >= 64){

        //Log attempted register write
        vpx3_log_err(vpx3, 2, reg);
        return;
    }
    vpx3->registers[reg] = val;
}



//[[ MEMORY ]]
uint8_t vpx3_mem_r8(Vpx3* vpx3, uint32_t adr){
    if(adr >= vpx3->mem_size){
        vpx3_log_err(vpx3, 3, adr); //log code and value
        return 0;
    }
    return vpx3->mem_ptr[adr];
}
uint16_t vpx3_mem_r16(Vpx3* vpx3, uint32_t adr){
    if(adr >= vpx3->mem_size - 2){
        vpx3_log_err(vpx3, 4, adr); //log code and value
        return 0;
    }
    uint16_t ds;
    memcpy(&ds, &vpx3->mem_ptr[adr], 2);
    return vpx3_16b_endian_fmt(ds);
}
uint32_t vpx3_mem_r32(Vpx3* vpx3, uint32_t adr){
    if(adr >= vpx3->mem_size - 4){
        vpx3_log_err(vpx3, 5, adr); //log code and value
        return 0;
    }
    uint32_t ds;
    memcpy(&ds, &vpx3->mem_ptr[adr], 4);
    return vpx3_32b_endian_fmt(ds);
}

void vpx3_mem_w8(Vpx3* vpx3, uint32_t adr, uint8_t val){
    if(adr >= vpx3->mem_size){
        vpx3_log_err(vpx3, 6, adr); //log code and value
        return;
    }
    vpx3->mem_ptr[adr] = val;
}
void vpx3_mem_w16(Vpx3* vpx3, uint32_t adr, uint16_t val){
    if(adr >= vpx3->mem_size - 2){
        vpx3_log_err(vpx3, 7, adr); //log code and value
        return;
    }
    uint16_t tmp = vpx3_16b_endian_fmt(val);
    memcpy(&vpx3->mem_ptr[adr], &tmp, 2);
}
void vpx3_mem_w32(Vpx3* vpx3, uint32_t adr, uint32_t val){
    if(adr >= vpx3->mem_size - 4){
        vpx3_log_err(vpx3, 8, adr); //log code and value
        return;
    }
    uint32_t tmp = vpx3_32b_endian_fmt(val);
    memcpy(&vpx3->mem_ptr[adr], &tmp, 4);
}



//[[ CONCIDERING THEY USE WRITE AND READ FUNCTIONS, IT IS NOT REQUIRED TO MAKE SEPERATE SAFE AND UNSAFE VARIANTS ]]
void vpx3_mem_pu8(Vpx3* vpx3, uint8_t val){
    //Push 8 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    //Write then increment.
    vpx3_mem_w8(vpx3, adr, val);
    vpx3_wreg(vpx3, VPX_RSP, adr+1);
}
void vpx3_mem_pu16(Vpx3* vpx3, uint16_t val){
    //Push 16 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    uint16_t tmp = vpx3_16b_endian_fmt(val);
    //Write then increment.
    vpx3_mem_w16(vpx3, adr, tmp);
    vpx3_wreg(vpx3, VPX_RSP, adr+2);
}
void vpx3_mem_pu32(Vpx3* vpx3, uint32_t val){
    //Push 32 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    uint32_t tmp = vpx3_32b_endian_fmt(val);
    //Write then increment.
    vpx3_mem_w32(vpx3, adr, tmp);
    vpx3_wreg(vpx3, VPX_RSP, adr+4);
}


uint8_t vpx3_mem_po8(Vpx3* vpx3){
    //pop 8 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    //Decrement then read
    vpx3_wreg(vpx3, VPX_RSP, adr-1);

    return vpx3_mem_r8(vpx3, adr-1);
}
uint16_t vpx3_mem_po16(Vpx3* vpx3){
    //pop 16 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    //Decrement then read
    vpx3_wreg(vpx3, VPX_RSP, adr-2);

    return vpx3_16b_endian_fmt(vpx3_mem_r16(vpx3, adr-2));
}
uint32_t vpx3_mem_po32(Vpx3* vpx3){
    //pop 32 bit
    uint32_t adr = vpx3_rreg(vpx3, VPX_RSP);
    //Decrement then read
    vpx3_wreg(vpx3, VPX_RSP, adr-4);

    return vpx3_32b_endian_fmt(vpx3_mem_r32(vpx3, adr-4));
}

//[[ FETCH (basically pop but using RPC sorta) ]]

uint8_t vpx3_mem_f8(Vpx3* vpx3){
    uint32_t adr = vpx3_rreg(vpx3, VPX_RPC);
    uint8_t val = vpx3_mem_r8(vpx3, adr);
    vpx3_wreg(vpx3, VPX_RPC, adr+1);
    return val;
}
uint16_t vpx3_mem_f16(Vpx3* vpx3){
    uint32_t adr = vpx3_rreg(vpx3, VPX_RPC);
    uint16_t val = vpx3_mem_r16(vpx3, adr);
    vpx3_wreg(vpx3, VPX_RPC, adr+2);
    return val;
}
uint32_t vpx3_mem_f32(Vpx3* vpx3){
    uint32_t adr = vpx3_rreg(vpx3, VPX_RPC);
    uint32_t val = vpx3_mem_r32(vpx3, adr);
    vpx3_wreg(vpx3, VPX_RPC, adr+4);
    return val;
}


//[[ ISA SECTION ]]

//[[ ISA INSTRUCTIONS ]]

static inline void vpx3_isa_mov(Vpx3* vpx3){
    //===========================================
    //Move value in r1 to r2.
    //===========================================
    //C syntax: registers[r1] = registers[r2];
    //Pseudocode: r1 <- r2
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t val = vpx3_rreg(vpx3, r2);
    vpx3_wreg(vpx3, r1, val);
    

}
static inline void vpx3_isa_movi(Vpx3* vpx3){
    //===========================================
    //Move immediate value to r1
    //===========================================
    //C syntax: registers[r1] = imm;
    //Pseudocode: r1 <- imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);
    vpx3_wreg(vpx3, r1, imm);
}
static inline void vpx3_isa_inc(Vpx3* vpx3){
    //===========================================
    //Increment value of r1
    //===========================================
    //C syntax: registers[r1] = registers[r1]++;
    //Pseudocode: r1 <- r1 + 1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val = vpx3_rreg(vpx3, r1);
    vpx3_wreg(vpx3, r1, val + 1);
}
static inline void vpx3_isa_dec(Vpx3* vpx3){
    //===========================================
    //Decrement value of r1
    //===========================================
    //C syntax: registers[r1] = registers[r1]--;
    //Pseudocode: r1 <- r1 - 1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val = vpx3_rreg(vpx3, r1);
    vpx3_wreg(vpx3, r1, val - 1);
}

static inline void vpx3_isa_or(Vpx3* vpx3){
    //===========================================
    //Do an OR operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] | registers[r3];
    //Pseudocode: r1 <- r2 or r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 | val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_xor(Vpx3* vpx3){
    //===========================================
    //Do an XOR operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] ^ registers[r3];
    //Pseudocode: r1 <- r2 xor r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 ^ val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_and(Vpx3* vpx3){
    //===========================================
    //Do an AND operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] & registers[r3];
    //Pseudocode: r1 <- r2 and r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 & val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_not(Vpx3* vpx3){
    //===========================================
    //Do a NOT operation on r2 and write to r1
    //===========================================
    //C syntax: registers[r1] = ~registers[r2];
    //Pseudocode: r1 <- not r2
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = ~val2;
    vpx3_wreg(vpx3, r1, val1);
}

static inline void vpx3_isa_ori(Vpx3* vpx3){
    //===========================================
    //Do an OR operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] | imm;
    //Pseudocode: r1 <- r2 or imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 | imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_xori(Vpx3* vpx3){
    //===========================================
    //Do an XOR operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] ^ imm;
    //Pseudocode: r1 <- r2 xor imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 ^ imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_andi(Vpx3* vpx3){
    //===========================================
    //Do an AND operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] & imm;
    //Pseudocode: r1 <- r2 and imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 & imm;
    vpx3_wreg(vpx3, r1, val1);
}

static inline void vpx3_isa_sll(Vpx3* vpx3){
    //===========================================
    //Shift logical left of r2 by r3 and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] << registers[r3];
    //Pseudocode: r1 <- r2 << r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 << val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_srl(Vpx3* vpx3){
    //===========================================
    //Shift logical right of r2 by r3 and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> registers[r3];
    //Pseudocode: r1 <- r2 >> r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 >> val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_sra(Vpx3* vpx3){
    //===========================================
    //Shift arithmetic right of r2 by r3 and write to r1
    //WARNING: might not work always!
    //might add inline assembly version if this doesn't work. with preproccessor checks
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> registers[r3];
    //Pseudocode: r1 <- r2 >> r3
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    int32_t val1 = (int32_t)val2 >> (int32_t)val3;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}

static inline void vpx3_isa_slli(Vpx3* vpx3){
    //===========================================
    //Immediate Shift logical left of r2 by imm and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] << imm;
    //Pseudocode: r1 <- r2 << imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t imm = vpx3_mem_f8(vpx3); //imm is 8 bits because you can't shift by more anyway

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 << imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_srli(Vpx3* vpx3){
    //===========================================
    //Immediate Shift logical right of r2 by imm and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> imm;
    //Pseudocode: r1 <- r2 >> imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t imm = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
 

    uint32_t val1 = val2 >> imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_srai(Vpx3* vpx3){
    //===========================================
    //Immediate Shift arithmetic right of r2 by imm and write to r1
    //WARNING: might not work always!
    //might add inline assembly version if this doesn't work. with preproccessor checks
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> imm;
    //Pseudocode: r1 <- r2 >> imm
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t imm = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    int32_t val1 = (int32_t)val2 >> (int32_t)imm;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}

static inline void vpx3_isa_add(Vpx3* vpx3){
    //===========================================
    //Add r2 and r3, write result to r1. (No carry)
    //===========================================
    //C syntax: registers[r1] = registers[r2] + registers[r3];
    //Pseudocode: r1 <- r2 + r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 + val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_sub(Vpx3* vpx3){
    //===========================================
    //Subtract r2 by r3, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] - registers[r3];
    //Pseudocode: r1 <- r2 - r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 - val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_mul(Vpx3* vpx3){
    //===========================================
    //Multiply r2 by r3, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] * registers[r3];
    //Pseudocode: r1 <- r2 * r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t val1 = val2 * val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_udiv(Vpx3* vpx3){
    //===========================================
    //Divide r2 by r3, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / registers[r3];
    //Pseudocode: r1 <- r2 / r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);
    if(val3 == 0){
        //Division by 0 error
        //Log aswell the register that contained it.
        vpx3_log_err(vpx3, VPX_ERR_DIV_BY_ZERO, r3);
        return;
    }

    uint32_t val1 = val2 / val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_sdiv(Vpx3* vpx3){
    //===========================================
    //Divide r2 by r3, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / registers[r3];
    //Pseudocode: r1 <- r2 / r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);
    //Signed version is stricter.
    if((val3 == 0) || ((int32_t)val2 == UINT32_MAX && (int32_t)val2 == -1)){
        //Division by 0 error (But signed)
        //Log aswell the register that contained it.
        vpx3_log_err(vpx3, VPX_ERR_DIV_BY_ZERO_S, r3);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)val3 == -1){
        vpx3_log_err(vpx3, VPX_ERR_DIV_INT32_MAX_N1, r2); //Signed conversion error.
        return;
    }


    int32_t val1 = (int32_t)val2 / (int32_t)val3;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}
static inline void vpx3_isa_urem(Vpx3* vpx3){
    //===========================================
    //Modulo/Remainder of r2 by r3, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % registers[r3];
    //Pseudocode: r1 <- r2 % r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);
    if(val3 == 0){
        //Division by 0 error
        //Log aswell the register that contained it.
        vpx3_log_err(vpx3, 9, r3);
        return;
    }

    uint32_t val1 = val2 % val3;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_srem(Vpx3* vpx3){
    //===========================================
    //Modulo/Remainder of r2 by r3, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % registers[r3];
    //Pseudocode: r1 <- r2 % r3
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    //Signed version is stricter.
    if(val3 == 0){
        //Division by 0 error (But signed)
        //Log aswell the register that contained it.
        vpx3_log_err(vpx3, 10, r3);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)val3 == -1){
        vpx3_log_err(vpx3, 11, r2);
        return;
    }


    int32_t val1 = (int32_t)val2 % (int32_t)val3;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}

static inline void vpx3_isa_addi(Vpx3* vpx3){
    //===========================================
    //Add r2 and imm, write result to r1. (No carry)
    //===========================================
    //C syntax: registers[r1] = registers[r2] + imm;
    //Pseudocode: r1 <- r2 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 + imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_subi(Vpx3* vpx3){
    //===========================================
    //Subtract r2 and imm, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] - imm;
    //Pseudocode: r1 <- r2 - imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 - imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_muli(Vpx3* vpx3){
    //===========================================
    //Multiply r2 by imm, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] * imm;
    //Pseudocode: r1 <- r2 * imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t val1 = val2 * imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_udivi(Vpx3* vpx3){
    //===========================================
    //Divide r2 by imm, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / imm;
    //Pseudocode: r1 <- r2 / imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    if(imm == 0){
        //Division by 0 error
        vpx3_log_err(vpx3, VPX_ERR_DIV_BY_ZERO, 0);
        return;
    }

    uint32_t val1 = val2 / imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_sdivi(Vpx3* vpx3){
    //===========================================
    //Divide r2 by imm, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / imm;
    //Pseudocode: r1 <- r2 / imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    //Signed version is stricter.
    if(imm == 0){
        //Division by 0 error (But signed)
        vpx3_log_err(vpx3, VPX_ERR_DIV_BY_ZERO_S, 0);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)imm == -1){
        vpx3_log_err(vpx3, VPX_ERR_DIV_INT32_MAX_N1, r2); //Signed conversion error.
        return;
    }


    int32_t val1 = (int32_t)val2 / (int32_t)imm;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}
static inline void vpx3_isa_uremi(Vpx3* vpx3){
    //===========================================
    //Modulo/Remainder of r2 by imm, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % imm;
    //Pseudocode: r1 <- r2 % imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    if(imm == 0){
        //Division by 0 error
        vpx3_log_err(vpx3, 9, 0);
        return;
    }

    uint32_t val1 = val2 % imm;
    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_sremi(Vpx3* vpx3){
    //===========================================
    //Modulo/Remainder of r2 by imm, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % imm;
    //Pseudocode: r1 <- r2 % imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    //Signed version is stricter.
    if(imm == 0){
        //Division by 0 error (But signed)
        vpx3_log_err(vpx3, 10, 0);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)imm == -1){
        vpx3_log_err(vpx3, 11, r2);
        return;
    }


    int32_t val1 = (int32_t)val2 % (int32_t)imm;
    vpx3_wreg(vpx3, r1, (uint32_t)val1);
}
//[[ ALL INSTRUCTIONS PAST THIS POINT ARE TO BE REMADE ]

//[[NOTES:]]
    //There are multiple versions of (almost) all memory operations (jmp, ld, st, call), Being:
    //[ins][n]i = I variant means immediate, address = (base)r0 + (offset)imm
    //Note: you can logically make imm the base and r0 the offset if you want absolute addressing
    //[ins][n]r = R variant means relative, Provided with address = (base)r1 + (offset)r2


//=IMMEDIATE=
static inline void vpx3_isa_ld8i(Vpx3* vpx3){
    //all these instructions, even if have the same aliases as from older versions, ARE different.
    //===========================================
    //Read 8 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r8(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    uint8_t val1 = vpx3_mem_r8(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}
static inline void vpx3_isa_ld16i(Vpx3* vpx3){
    //===========================================
    //Read 16 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r16(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    uint16_t val1 = vpx3_mem_r16(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}
static inline void vpx3_isa_ld32i(Vpx3* vpx3){
    //===========================================
    //Read 32 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r32(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    uint32_t val1 = vpx3_mem_r32(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}
static inline void vpx3_isa_ld8r(Vpx3* vpx3){
    //===========================================
    //Read 8 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r8(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);


    uint32_t adr = val2 + val3;

    uint8_t val1 = vpx3_mem_r8(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}
static inline void vpx3_isa_ld16r(Vpx3* vpx3){
    //===========================================
    //Read 16 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r16(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);


    uint32_t adr = val2 + val3;

    uint16_t val1 = vpx3_mem_r16(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}
static inline void vpx3_isa_ld32r(Vpx3* vpx3){
    //===========================================
    //Read 32 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r32(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);


    uint32_t adr = val2 + val3;

    uint32_t val1 = vpx3_mem_r32(vpx3, adr);
    vpx3_wreg(vpx3, r1, val1);

}

static inline void vpx3_isa_st8i(Vpx3* vpx3){
    //all these instructions, even if have the same aliases as from older versions, ARE different.
    //===========================================
    //Store 8 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w8(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    vpx3_mem_w8(vpx3, adr, val1 & 0xFF);
}
static inline void vpx3_isa_st16i(Vpx3* vpx3){
    //===========================================
    //Store 16 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w16(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    vpx3_mem_w16(vpx3, adr, val1 & 0xFFFF);
}
static inline void vpx3_isa_st32i(Vpx3* vpx3){
    //===========================================
    //Store 32 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w32(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t adr = val2 + imm;

    vpx3_mem_w32(vpx3, adr, val1);
}
static inline void vpx3_isa_st8r(Vpx3* vpx3){
    //===========================================
    //Store 8 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w8(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t adr = val2 + val3;

    vpx3_mem_w8(vpx3, adr, val1 & 0xFF);
}
static inline void vpx3_isa_st16r(Vpx3* vpx3){
    //===========================================
    //Store 16 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w16(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t adr = val2 + val3;

    vpx3_mem_w16(vpx3, adr, val1 & 0xFFFF);
}
static inline void vpx3_isa_st32r(Vpx3* vpx3){
    //===========================================
    //Store 32 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w32(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t val3 = vpx3_rreg(vpx3, r3);

    uint32_t adr = val2 + val3;

    vpx3_mem_w32(vpx3, adr, val1);
}

static inline void vpx3_isa_jmpi(Vpx3* vpx3){
    //all these instructions, even if have the same aliases as from older versions, ARE different.
    //NOTE:
    //this instruction can be replaced with: addi, rpc, r1, imm, But it's prefferable to use this
    //For the sake of simplicity!
    //===========================================
    //Jump to address defined by: PC = (r1 + imm)
    //===========================================
    //C syntax: registers[rpc] = registers[r1] + imm;
    //Pseudocode: rpc = r1 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);

    uint32_t valrpc = val1 + imm;
    vpx3_wreg(vpx3, VPX_RPC, valrpc);
}
static inline void vpx3_isa_jmpr(Vpx3* vpx3){

    //this instruction can be replaced with: add, rpc, r1, r2, But it's prefferable to use this
    //For the sake of simplicity!
    //===========================================
    //Jump to address defined by: PC = (r1 + r2)
    //===========================================
    //C syntax: registers[rpc] = registers[r1] + registers[r2];
    //Pseudocode: rpc = r1 + r2
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);

    uint32_t valrpc = val1 + val2;
    vpx3_wreg(vpx3, VPX_RPC, valrpc);
}

static inline void vpx3_isa_zjmpi(Vpx3* vpx3){
    //===========================================
    //Conditional Jump to address defined by: PC = (r2 + imm) IF: r1 == 0
    //===========================================
    //C syntax: if(registers[r1] == 0){registers[rpc] = registers[r2] + imm;}
    //Pseudocode: if r1 = 0: rpc = r2 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);

    if(val1 == 0){
        uint32_t val2 = vpx3_rreg(vpx3, r2);
        uint32_t valrpc = val2 + imm;
        vpx3_wreg(vpx3, VPX_RPC, valrpc);
    }

}
static inline void vpx3_isa_eqjmpi(Vpx3* vpx3){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 = r2
    //===========================================
    //C syntax: if(registers[r1] == registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 = r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    
    if(val1 == val2){
        uint32_t val3 = vpx3_rreg(vpx3, r3);
        uint32_t valrpc = val3 + imm;
        vpx3_wreg(vpx3, VPX_RPC, valrpc);
    }

}
static inline void vpx3_isa_nejmpi(Vpx3* vpx3){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 != r2
    //===========================================
    //C syntax: if(registers[r1] != registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 != r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    
    if(val1 != val2){
        uint32_t val3 = vpx3_rreg(vpx3, r3);
        uint32_t valrpc = val3 + imm;
        vpx3_wreg(vpx3, VPX_RPC, valrpc);
    }

}
static inline void vpx3_isa_gjmpi(Vpx3* vpx3){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 > r2
    //===========================================
    //C syntax: if(registers[r1] > registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 > r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    
    if(val1 > val2){
        uint32_t val3 = vpx3_rreg(vpx3, r3);
        uint32_t valrpc = val3 + imm;
        vpx3_wreg(vpx3, VPX_RPC, valrpc);
    }

}
static inline void vpx3_isa_gejmpi(Vpx3* vpx3){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 >= r2
    //===========================================
    //C syntax: if(registers[r1] >= registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 >= r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint8_t r3 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    
    if(val1 >= val2){
        uint32_t val3 = vpx3_rreg(vpx3, r3);
        uint32_t valrpc = val3 + imm;
        vpx3_wreg(vpx3, VPX_RPC, valrpc);
    }

}

static inline void vpx3_isa_push8(Vpx3* vpx3){
    //===========================================
    //Push 8 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu8(registers[r1] & 0xff);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_rreg(vpx3, r1);

    vpx3_mem_pu8(vpx3, val1 & 0xff);
}
static inline void vpx3_isa_push16(Vpx3* vpx3){
    //===========================================
    //Push 16 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu16(registers[r1] & 0xffff);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_rreg(vpx3, r1);

    vpx3_mem_pu16(vpx3, val1 & 0xffff);
}
static inline void vpx3_isa_push32(Vpx3* vpx3){
    //===========================================
    //Push 32 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu32(registers[r1]);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_rreg(vpx3, r1);

    vpx3_mem_pu32(vpx3, val1);
}
static inline void vpx3_isa_pop8(Vpx3* vpx3){
    //===========================================
    //Pop 8 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po8();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_mem_po8(vpx3);

    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_pop16(Vpx3* vpx3){
    //===========================================
    //Pop 16 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po16();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_mem_po16(vpx3);

    vpx3_wreg(vpx3, r1, val1);
}
static inline void vpx3_isa_pop32(Vpx3* vpx3){
    //===========================================
    //Pop 32 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po32();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t val1 = vpx3_mem_po32(vpx3);

    vpx3_wreg(vpx3, r1, val1);
}

static inline void vpx3_isa_calli(Vpx3* vpx3){
    //===========================================
    //Jump to address defined by: PC = (r1 + imm) And push current address to Stack (after fetching)
    //===========================================
    //C syntax: mem_pu32(registers[rpc]); registers[rpc] = registers[r1] + imm;
    //Pseudocode: stack[rsp] = rpc; rsp++; rpc = r1 + imm
    //===========================================
    //No need for adding pseudo code descriptions for these, Since it's a bit obvious.
    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint32_t imm = vpx3_mem_f32(vpx3);
    uint32_t ret_adr = vpx3_rreg(vpx3, VPX_RPC) - 1; //Might be an off by 1 error. Change (-1) if so is the case.

    //PUSH
    vpx3_mem_pu32(vpx3, ret_adr);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t valrpc = val1 + imm;

    vpx3_wreg(vpx3, VPX_RPC, valrpc);
}
static inline void vpx3_isa_callr(Vpx3* vpx3){
    //===========================================
    //Jump to address defined by: PC = (r1 + r2) And push current address to Stack (after fetching)
    //===========================================

    uint8_t r1 = vpx3_mem_f8(vpx3);
    uint8_t r2 = vpx3_mem_f8(vpx3);
    uint32_t ret_adr = vpx3_rreg(vpx3, VPX_RPC) - 1; //Might be an off by 1 error. Change (-1) if so is the case.

    //PUSH
    vpx3_mem_pu32(vpx3, ret_adr);

    uint32_t val1 = vpx3_rreg(vpx3, r1);
    uint32_t val2 = vpx3_rreg(vpx3, r2);
    uint32_t valrpc = val1 + val2;

    vpx3_wreg(vpx3, VPX_RPC, valrpc);
}
static inline void vpx3_isa_ret(Vpx3* vpx3){
    //===========================================
    //Jump to address in first 32 bits from stack (popped)
    //===========================================

    uint32_t ret_adr = vpx3_mem_po32(vpx3);


    vpx3_wreg(vpx3, VPX_RPC, ret_adr);
}

//[[ EXTENDED ISA ]]

//=FPU=

//[[ ISA PRIMARY EXEC ]]


uint8_t vpx3_exec(Vpx3* vpx3){
    //Triggers error on invalid opcode.
    uint8_t opcode = vpx3_mem_f8(vpx3); //Fetch opcode.

    uint8_t rv = 0;

    switch(opcode){
        default: {
            //Log error and exit.
            vpx3_log_err(vpx3, VPX_ERR_INVALID_OPCODE, opcode);
            
            rv = 1; //Error!

            break;

        }
        case 0: rv = 0; break; //Does nothing, NOP
        case 1: rv = 255; break; //Hostcall.
        case 2: rv = 0; break; //Depracated instruction, Does nothing now.
        case 3: vpx3_isa_mov(vpx3); break;
        case 4: vpx3_isa_movi(vpx3); break;
        case 5: vpx3_isa_inc(vpx3); break;
        case 6: vpx3_isa_dec(vpx3); break;
        case 7: vpx3_isa_or(vpx3); break;
        case 8: vpx3_isa_xor(vpx3); break;
        case 9: vpx3_isa_and(vpx3); break;
        case 10: vpx3_isa_not(vpx3); break;
        case 11: vpx3_isa_ori(vpx3); break;
        case 12: vpx3_isa_xori(vpx3); break;
        case 13: vpx3_isa_andi(vpx3); break;
        case 14: vpx3_isa_sll(vpx3); break;
        case 15: vpx3_isa_srl(vpx3); break;
        case 16: vpx3_isa_sra(vpx3); break;
        case 17: vpx3_isa_slli(vpx3); break;
        case 18: vpx3_isa_srli(vpx3); break;
        case 19: vpx3_isa_srai(vpx3); break;
        case 20: vpx3_isa_add(vpx3); break;
        case 21: vpx3_isa_sub(vpx3); break;
        case 22: vpx3_isa_mul(vpx3); break;
        case 23: vpx3_isa_udiv(vpx3); break;
        case 24: vpx3_isa_sdiv(vpx3); break;
        case 25: vpx3_isa_urem(vpx3); break;
        case 26: vpx3_isa_srem(vpx3); break;
        case 27: vpx3_isa_addi(vpx3); break;
        case 28: vpx3_isa_subi(vpx3); break;
        case 29: vpx3_isa_muli(vpx3); break;
        case 30: vpx3_isa_udivi(vpx3); break;
        case 31: vpx3_isa_sdivi(vpx3); break;
        case 32: vpx3_isa_uremi(vpx3); break;
        case 33: vpx3_isa_sremi(vpx3); break;
        //[[ REMAKING ALL INSTRUCTIONS PAST THIS POINT (v2) ]]
        case 34: vpx3_isa_ld8i(vpx3); break;
        case 35: vpx3_isa_ld16i(vpx3); break;
        case 36: vpx3_isa_ld32i(vpx3); break;
        case 37: vpx3_isa_ld8r(vpx3); break;
        case 38: vpx3_isa_ld16r(vpx3); break;
        case 39: vpx3_isa_ld32r(vpx3); break;
        case 40: vpx3_isa_st8i(vpx3); break;
        case 41: vpx3_isa_st16i(vpx3); break;
        case 42: vpx3_isa_st32i(vpx3); break;
        case 43: vpx3_isa_st8r(vpx3); break;
        case 44: vpx3_isa_st16r(vpx3); break;
        case 45: vpx3_isa_st32r(vpx3); break;
        case 46: vpx3_isa_jmpi(vpx3); break;
        case 47: vpx3_isa_jmpr(vpx3); break;
        case 48: vpx3_isa_zjmpi(vpx3); break;
        case 49: vpx3_isa_eqjmpi(vpx3); break;
        case 50: vpx3_isa_nejmpi(vpx3); break;
        case 51: vpx3_isa_gjmpi(vpx3); break;
        case 52: vpx3_isa_gejmpi(vpx3); break;
        case 53: vpx3_isa_push8(vpx3); break;
        case 54: vpx3_isa_push16(vpx3); break;
        case 55: vpx3_isa_push32(vpx3); break;
        case 56: vpx3_isa_pop8(vpx3); break;
        case 57: vpx3_isa_pop16(vpx3); break;
        case 58: vpx3_isa_pop32(vpx3); break;
        case 59: vpx3_isa_calli(vpx3); break;
        case 60: vpx3_isa_callr(vpx3); break;
        case 61: vpx3_isa_ret(vpx3); break; //5 less instructions from original.




        //[[ EXTENDED ISA ]]
    }
    if(vpx3->err_code){rv = 1;} //error!
    return rv; //Successful execution
}

//[[ PRIMARY FUNCTIONS ]]

uint8_t vpx3_init(Vpx3* vpx3, uint8_t* mem_ptr, uint32_t mem_size){
    if(mem_ptr == VPXNULL){
        return 1; //Fail
    }
    if(mem_size == 0){
        return 1; //Fail
    }
    vpx3->mem_ptr = mem_ptr;
    vpx3->mem_size = mem_size;

    return 0; //Success


}
uint8_t vpx3_start(Vpx3* vpx3){
    while(1){
        uint8_t rt = vpx3_exec(vpx3);
        if(rt == 1){return 1;} //error exit
        if(rt == 255){return 0;} //hostcall successful exit.

    }
    return 0;


}
