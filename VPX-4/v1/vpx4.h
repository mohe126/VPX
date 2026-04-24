#pragma once

//[[ C STANDARD LIB DEPENDENCIES ]]
#include <string.h>
#include <stdint.h>

//[[ ISA DEPENDENCY ]]

#include "vpx4-isa.h"

//[[ MACRO ]]
#ifdef VPX_BIG_ENDIAN

static inline uint32_t vpx4_endian_fmt32(uint32_t val){
    return __builtin_bswap32(val);
}
static inline uint32_t vpx4_endian_fmt16(uint16_t val){
    return __builtin_bswap16(val);
}
#else
static inline uint32_t vpx4_endian_fmt32(uint32_t val){
    return val;
}
static inline uint32_t vpx4_endian_fmt16(uint16_t val){
    return val;
}
#endif

//=== REGISTERS ===
#define VPX4_RPC 62
#define VPX4_RSP 63



//=== ERROR/EXCEPTION CODES ===
#define VPX4_E_RREG 1
#define VPX4_E_WREG 2

#define VPX4_E_MW08 3 //memory write 8 bit
#define VPX4_E_MW16 4
#define VPX4_E_MW32 5

#define VPX4_E_MR08 6
#define VPX4_E_MR16 7
#define VPX4_E_MR32 8

#define VPX4_E_INVOP 9

#define VPX4_E_DB0 10 //Divide by Zero
#define VPX4_E_DB0S 11 //Divide by Zero Signed
#define VPX4_E_DI32M 12 //INT32_MAX division error



//[[ TYPEDEFs ]]
struct Vpx4;

typedef uint8_t (*vpx4_instruction_t)(Vpx4*);


//[[ STRUCTS ]]

typedef struct {
    uint32_t code;
    uint32_t value;


} vpx4_error_t;
typedef struct {
    uint8_t* ptr;
    uint32_t alloc;

} vpx4_memory_t;
typedef struct {
    uint32_t* ptr;
    uint8_t count;

} vpx4_register_t;



typedef struct {
    //[[ ERROR ]]
    vpx4_error_t error;


    //[[ ISA ]]
    vpx4_instruction_t isa[256];

    //[[ REGISTERS ]]
    uint32_t default_registers[64];
    
    vpx4_register_t registers;

    //[[ MEMORY ]]
    vpx4_memory_t mem;

    //[[ HOSTCALL ]]
    uint8_t hostcall;





} Vpx4;




//[[ CONFIG FUNCTIONS ]]
void vpx4_config_isa(Vpx4* vpx4, uint8_t opcode, vpx4_instruction_t ins);
void vpx4_config_reg(Vpx4* vpx4, uint32_t* ptr, uint8_t count);
void vpx4_config_mem(Vpx4* vpx4, uint8_t* ptr, uint32_t alloc);

//[[ ERROR FUNCTIONS ]]
void vpx4_err(Vpx4* vpx4, uint8_t code, uint32_t value);

//[[ REGISTER FUNCTIONS ]]
uint32_t vpx4_rreg(Vpx4* vpx4, uint8_t r);
void vpx4_wreg(Vpx4* vpx4, uint8_t r, uint32_t val);



//[[ MEMORY FUNCTIONS ]]
void vpx4_mem_w08(Vpx4* vpx4, uint32_t adr, uint8_t val);
void vpx4_mem_w16(Vpx4* vpx4, uint32_t adr, uint16_t val);
void vpx4_mem_w32(Vpx4* vpx4, uint32_t adr, uint32_t val);

uint8_t vpx4_mem_r08(Vpx4* vpx4, uint32_t adr);
uint16_t vpx4_mem_r16(Vpx4* vpx4, uint32_t adr);
uint32_t vpx4_mem_r32(Vpx4* vpx4, uint32_t adr);

void vpx4_mem_pu08(Vpx4* vpx4, uint8_t val);
void vpx4_mem_pu16(Vpx4* vpx4, uint16_t val);
void vpx4_mem_pu32(Vpx4* vpx4, uint32_t val);

uint8_t vpx4_mem_po08(Vpx4* vpx4);
uint16_t vpx4_mem_po16(Vpx4* vpx4);
uint32_t vpx4_mem_po32(Vpx4* vpx4);

uint8_t vpx4_mem_f08(Vpx4* vpx4);
uint16_t vpx4_mem_f16(Vpx4* vpx4);
uint32_t vpx4_mem_f32(Vpx4* vpx4);



//[[ SETUP FUNCTIONS ]]

uint8_t vpx4_exec(Vpx4* vpx4);
void vpx4_init(Vpx4* vpx4, uint8_t* mem_ptr, uint32_t mem_alloc);


