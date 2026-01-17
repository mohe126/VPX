#pragma once

#include <stdint.h>

//[[ PRIMARY ]]
void vpx1000_init(uint8_t* ptr, uint32_t size, uint8_t mode);
uint8_t vpx1000_step();
uint8_t vpx1000_start();

extern uint8_t vpx1000_f_safe;
extern uint8_t vpx1000_f_condition_met;
extern uint8_t vpx1000_f_hostcall;

extern uint8_t vpx1000_f_error_code;
extern uint32_t vpx1000_f_error_value;

//[[ MACRO ]]
#define vpx1000_NULL 0
#define vpx1000_RPC 253
#define vpx1000_RSP 254
#define vpx1000_RHC 255


//[[ CPU ]]

extern uint32_t vpx1000_cpu_registers[256];
uint32_t vpx1000_cpu_rreg(uint32_t reg);
void vpx1000_cpu_wreg(uint8_t reg, uint32_t val);

//[[ MEM ]]
extern uint8_t* vpx1000_mem_ptr;
extern uint32_t vpx1000_mem_size;


extern uint8_t (*vpx1000_mem_read8)(uint32_t);
extern uint16_t (*vpx1000_mem_read16)(uint32_t);
extern uint32_t (*vpx1000_mem_read32)(uint32_t);

extern void (*vpx1000_mem_write8)(uint32_t, uint8_t);
extern void (*vpx1000_mem_write16)(uint32_t, uint16_t);
extern void (*vpx1000_mem_write32)(uint32_t, uint32_t);

extern uint8_t (*vpx1000_mem_fetch8)(void);
extern uint16_t (*vpx1000_mem_fetch16)(void);
extern uint32_t (*vpx1000_mem_fetch32)(void);

extern void (*vpx1000_mem_push8)(uint8_t);
extern void (*vpx1000_mem_push16)(uint16_t);
extern void (*vpx1000_mem_push32)(uint32_t);

extern uint8_t (*vpx1000_mem_pop8)(void);
extern uint16_t (*vpx1000_mem_pop16)(void);
extern uint32_t (*vpx1000_mem_pop32)(void);

void vpx1000_mem_safety_setup(uint8_t mode);
void vpx1000_mem_setup(uint8_t* ptr, uint32_t size, uint8_t safety_mode);

// [[ ISA ]]
extern uint8_t (*vpx1000_isa_exec)(void);
void vpx1000_isa_setup(uint8_t saftey_mode);