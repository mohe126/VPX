#pragma once


//[[ INCLUDES ]]
#include <stdint.h>
#include <string.h>

//[[ MACROS ]]
#define VPXNULL 0
#define VPX_RPC 62
#define VPX_RSP 63
//== error codes ==

#define VPX_ERR_RREG 1
#define VPX_ERR_WREG 2

#define VPX_ERR_MEM_R8 3
#define VPX_ERR_MEM_R16 4
#define VPX_ERR_MEM_R32 5

#define VPX_ERR_MEM_W8 6
#define VPX_ERR_MEM_W16 7
#define VPX_ERR_MEM_W32 8

#define VPX_ERR_DIV_BY_ZERO 9 //Float version has _F at the end.
#define VPX_ERR_DIV_BY_ZERO_S 10
#define VPX_ERR_DIV_INT32_MAX_N1 11
#define VPX_ERR_CJMP_INVALID 12

#define VPX_ERR_RREG_64 13
#define VPX_ERR_WREG_64 14

#define VPX_ERR_INVALID_OPCODE 15

//this is essentially for formatting, if the system is little endian it does nothing
//otherwise it swaps
#ifdef VPX_BIG_ENDIAN

static inline uint32_t vpx3_32b_endian_fmt(uint32_t val){
    return __builtin_bswap32(val);
}
static inline uint32_t vpx3_16b_endian_fmt(uint16_t val){
    return __builtin_bswap16(val);
}
#else
//Likely optimized away to nothingness
static inline uint32_t vpx3_32b_endian_fmt(uint32_t val){
    return val;
}
static inline uint32_t vpx3_16b_endian_fmt(uint16_t val){
    return val;
}
#endif


//[[ PRIMARY STRUCT ]]
struct Vpx3 {
    //[[ ERROR ]]
    uint8_t err_code;
    uint32_t err_val;
    uint32_t err_pc_state;
    //[[ MEMORY ]]
    uint8_t* mem_ptr;
    uint32_t mem_size;
    uint32_t registers[64];
};
struct Vpx3;
typedef struct Vpx3 Vpx3;


//[[ INTERNAL FUNCTION DECLARATIONS ]]
//[[ ERROR LOG ]]
void vpx3_log_err(Vpx3* vpx3, uint8_t code, uint32_t value);

//[[ REGISTER ]]
uint32_t vpx3_rreg(Vpx3* vpx3, uint8_t reg);
void vpx3_wreg(Vpx3* vpx3, uint8_t reg, uint32_t val);

//[[ MEMORY ]]
uint8_t vpx3_mem_r8(Vpx3* vpx3, uint32_t adr);
uint16_t vpx3_mem_r16(Vpx3* vpx3, uint32_t adr);
uint32_t vpx3_mem_r32(Vpx3* vpx3, uint32_t adr);

void vpx3_mem_w8(Vpx3* vpx3, uint32_t adr, uint8_t val);
void vpx3_mem_w16(Vpx3* vpx3, uint32_t adr, uint16_t val);
void vpx3_mem_w32(Vpx3* vpx3, uint32_t adr, uint32_t val);

void vpx3_mem_pu8(Vpx3* vpx3, uint8_t val);
void vpx3_mem_pu16(Vpx3* vpx3, uint16_t val);
void vpx3_mem_pu32(Vpx3* vpx3, uint32_t val);


uint8_t vpx3_mem_po8(Vpx3* vpx3);
uint16_t vpx3_mem_po16(Vpx3* vpx3);
uint32_t vpx3_mem_po32(Vpx3* vpx3);

uint8_t vpx3_mem_f8(Vpx3* vpx3);
uint16_t vpx3_mem_f16(Vpx3* vpx3);
uint32_t vpx3_mem_f32(Vpx3* vpx3);


//[[ PRIMARY FUNCTION DECLARATIONS ]]
uint8_t vpx3_exec(Vpx3* vpx3);
uint8_t vpx3_init(Vpx3* vpx3, uint8_t* mem_ptr, uint32_t mem_size);
uint8_t vpx3_start(Vpx3* vpx3);


