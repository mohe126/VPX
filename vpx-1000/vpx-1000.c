//RISC ONLY VERSION (IDENTICAL OTHERWISE)
/*
LICENSE:

MIT License

Copyright (c) 2026 mohe126

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <stdint.h>
#include "vpx-1000.h"

//[[ MACROS ]]

#define vpx1000_NULL 0
#define vpx1000_RPC 253
#define vpx1000_RSP 254
#define vpx1000_RHC 255
//[[ GENERAL ]]

uint8_t vpx1000_f_safe = 0;
uint8_t vpx1000_f_condition_met = 0;
uint8_t vpx1000_f_hostcall = 0;



//[[ ERROR ]]
uint8_t vpx1000_f_error_code = 0; //Code of the error, So memory or ISA etc..
uint32_t vpx1000_f_error_value = 0; //Value of the error, like invalid address etc..


void vpx1000_error_set(uint8_t code, uint32_t value){
    vpx1000_f_error_code = code;
    vpx1000_f_error_value = value;
}


//[[ CPU ]]


uint32_t vpx1000_cpu_registers[256] = {
    0,
    //r253: RPC.
    //r254: RSP.
    //r255: RHC.
};

uint32_t vpx1000_cpu_rreg(uint32_t reg){
    return vpx1000_cpu_registers[reg];
}
void vpx1000_cpu_wreg(uint8_t reg, uint32_t val){
    vpx1000_cpu_registers[reg] = val;
}

//[[ MEMORY ]]
uint8_t* vpx1000_mem_ptr = vpx1000_NULL;
uint32_t vpx1000_mem_size = 0;


//main versions
uint8_t (*vpx1000_mem_read8)(uint32_t);
uint16_t (*vpx1000_mem_read16)(uint32_t);
uint32_t (*vpx1000_mem_read32)(uint32_t);

void (*vpx1000_mem_write8)(uint32_t, uint8_t);
void (*vpx1000_mem_write16)(uint32_t, uint16_t);
void (*vpx1000_mem_write32)(uint32_t, uint32_t);

uint8_t (*vpx1000_mem_fetch8)(void);
uint16_t (*vpx1000_mem_fetch16)(void);
uint32_t (*vpx1000_mem_fetch32)(void);

void (*vpx1000_mem_push8)(uint8_t);
void (*vpx1000_mem_push16)(uint16_t);
void (*vpx1000_mem_push32)(uint32_t);

uint8_t (*vpx1000_mem_pop8)(void);
uint16_t (*vpx1000_mem_pop16)(void);
uint32_t (*vpx1000_mem_pop32)(void);

//safe
uint8_t vpx1000_mem_sf_read8(uint32_t adr){
    if(adr >= vpx1000_mem_size){
        vpx1000_f_error_code = 1; //Memory read error.
        vpx1000_f_error_value = adr;
        return 0;
    }
    return vpx1000_mem_ptr[adr];

}
uint16_t vpx1000_mem_sf_read16(uint32_t adr){
    if(adr + 1 >= vpx1000_mem_size){
        vpx1000_f_error_code = 1; //Memory read error.
        vpx1000_f_error_value = adr;
        return 0;
    }
    return (vpx1000_mem_ptr[adr] << 8) | vpx1000_mem_ptr[adr+1];    
}
uint32_t vpx1000_mem_sf_read32(uint32_t adr){
    if(adr + 3 >= vpx1000_mem_size){
        vpx1000_f_error_code = 1; //Memory read error.
        vpx1000_f_error_value = adr;
        return 0;
    }
    return (vpx1000_mem_ptr[adr] << 24) | (vpx1000_mem_ptr[adr+1] << 16) | (vpx1000_mem_ptr[adr+2] << 8) | vpx1000_mem_ptr[adr+3];    
}

void vpx1000_mem_sf_write8(uint32_t adr, uint8_t val){
     if(adr >= vpx1000_mem_size){
        vpx1000_f_error_code = 2; //Memory write error.
        vpx1000_f_error_value = adr;
        return;
    }
    vpx1000_mem_ptr[adr] = val;
}
void vpx1000_mem_sf_write16(uint32_t adr, uint16_t val){
     if(adr+1 >= vpx1000_mem_size){
        vpx1000_f_error_code = 2; //Memory write error.
        vpx1000_f_error_value = adr;
        return;
    }
    vpx1000_mem_ptr[adr] = val >> 8; //MSB
    vpx1000_mem_ptr[adr+1] = val & 0xff;

}
void vpx1000_mem_sf_write32(uint32_t adr, uint32_t val){
     if(adr+3 >= vpx1000_mem_size){
        vpx1000_f_error_code = 2; //Memory write error.
        vpx1000_f_error_value = adr;
        return;
    }
    vpx1000_mem_ptr[adr] = val >> 24; //MSB
    vpx1000_mem_ptr[adr+1] = (val >> 16) & 0xff;
    vpx1000_mem_ptr[adr+2] = (val >> 8) & 0xff;
    vpx1000_mem_ptr[adr+3] = (val) & 0xff;

}

uint8_t vpx1000_mem_sf_fetch8(){
    //Reads before incrementing!

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 1);
    uint8_t val = vpx1000_mem_read8(adr);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 3; //Memory fetch error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;

}
uint16_t vpx1000_mem_sf_fetch16(){

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 2);

    uint16_t val = vpx1000_mem_read16(adr);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 3; //Memory fetch error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;
}
uint32_t vpx1000_mem_sf_fetch32(){

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 4);

    uint32_t val = vpx1000_mem_read32(adr);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 3; //Memory fetch error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;
}

void vpx1000_mem_sf_push8(uint8_t val){
    //Push value then increment.
    //For popping, you decrease then extract!
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write8(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 1);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 4; //Memory push error.
        vpx1000_f_error_value = adr;
    }


}
void vpx1000_mem_sf_push16(uint16_t val){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write16(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 2);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 4; //Memory push error.
        vpx1000_f_error_value = adr;
    }
    

}
void vpx1000_mem_sf_push32(uint32_t val){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write32(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 4);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 4; //Memory push error.
        vpx1000_f_error_value = adr;
    }
    

}

uint8_t vpx1000_mem_sf_pop8(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint8_t val = vpx1000_mem_sf_read8(adr-1);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-1);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 5; //Memory pop error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;
}
uint16_t vpx1000_mem_sf_pop16(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint16_t val = vpx1000_mem_sf_read16(adr-2);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-2);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 5; //Memory pop error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;
}
uint32_t vpx1000_mem_sf_pop32(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint32_t val = vpx1000_mem_sf_read32(adr-4);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-4);

    if(vpx1000_f_error_code){
        vpx1000_f_error_code = 5; //Memory pop error.
        vpx1000_f_error_value = adr;
        return 0;
    }

    return val;
}

//unsafe

uint8_t vpx1000_mem_uf_read8(uint32_t adr){
    //No memory bounds check.
    return vpx1000_mem_ptr[adr];

}
uint16_t vpx1000_mem_uf_read16(uint32_t adr){

    return (vpx1000_mem_ptr[adr] << 8) | vpx1000_mem_ptr[adr+1];    
}
uint32_t vpx1000_mem_uf_read32(uint32_t adr){

    return (vpx1000_mem_ptr[adr] << 24) | (vpx1000_mem_ptr[adr+1] << 16) | (vpx1000_mem_ptr[adr+2] << 8) | vpx1000_mem_ptr[adr+3];    
}

void vpx1000_mem_uf_write8(uint32_t adr, uint8_t val){

    vpx1000_mem_ptr[adr] = val;
}
void vpx1000_mem_uf_write16(uint32_t adr, uint16_t val){

    vpx1000_mem_ptr[adr] = val >> 8; //MSB
    vpx1000_mem_ptr[adr+1] = val & 0xff;

}
void vpx1000_mem_uf_write32(uint32_t adr, uint32_t val){

    vpx1000_mem_ptr[adr] = val >> 24; //MSB
    vpx1000_mem_ptr[adr+1] = (val >> 16) & 0xff;
    vpx1000_mem_ptr[adr+2] = (val >> 8) & 0xff;
    vpx1000_mem_ptr[adr+3] = (val) & 0xff;

}

uint8_t vpx1000_mem_uf_fetch8(){
    //Reads before incrementing!

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 1);
    uint8_t val = vpx1000_mem_read8(adr);


    return val;

}
uint16_t vpx1000_mem_uf_fetch16(){

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 2);

    uint16_t val = vpx1000_mem_read16(adr);


    return val;
}
uint32_t vpx1000_mem_uf_fetch32(){

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_cpu_wreg(vpx1000_RPC, adr + 4);

    uint32_t val = vpx1000_mem_read32(adr);



    return val;
}

void vpx1000_mem_uf_push8(uint8_t val){

    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write8(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 1);



}
void vpx1000_mem_uf_push16(uint16_t val){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write16(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 2);

}
void vpx1000_mem_uf_push32(uint32_t val){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    vpx1000_mem_sf_write32(adr, val);
    vpx1000_cpu_wreg(vpx1000_RSP, adr + 4);


}

uint8_t vpx1000_mem_uf_pop8(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint8_t val = vpx1000_mem_sf_read8(adr-1);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-1);



    return val;
}
uint16_t vpx1000_mem_uf_pop16(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint16_t val = vpx1000_mem_sf_read16(adr-2);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-2);


    return val;
}
uint32_t vpx1000_mem_uf_pop32(){
    uint32_t adr = vpx1000_cpu_rreg(vpx1000_RSP);
    uint32_t val = vpx1000_mem_sf_read32(adr-4);
    vpx1000_cpu_wreg(vpx1000_RSP, adr-4);


    return val;
}




void vpx1000_mem_safety_setup(uint8_t mode){
    if(mode){
        //Safe mode! Positive integer.
        vpx1000_mem_read8 = vpx1000_mem_sf_read8;
        vpx1000_mem_read16 = vpx1000_mem_sf_read16;
        vpx1000_mem_read32 = vpx1000_mem_sf_read32;

        vpx1000_mem_write8 = vpx1000_mem_sf_write8;
        vpx1000_mem_write16 = vpx1000_mem_sf_write16;
        vpx1000_mem_write32 = vpx1000_mem_sf_write32;

        vpx1000_mem_fetch8 = vpx1000_mem_sf_fetch8;
        vpx1000_mem_fetch16 = vpx1000_mem_sf_fetch16;
        vpx1000_mem_fetch32 = vpx1000_mem_sf_fetch32;

        vpx1000_mem_push8 = vpx1000_mem_sf_push8;
        vpx1000_mem_push16 = vpx1000_mem_sf_push16;
        vpx1000_mem_push32 = vpx1000_mem_sf_push32;

        vpx1000_mem_pop8 = vpx1000_mem_sf_pop8;
        vpx1000_mem_pop16 = vpx1000_mem_sf_pop16;
        vpx1000_mem_pop32 = vpx1000_mem_sf_pop32;
    }
    else{
        vpx1000_mem_read8 = vpx1000_mem_uf_read8;
        vpx1000_mem_read16 = vpx1000_mem_uf_read16;
        vpx1000_mem_read32 = vpx1000_mem_uf_read32;

        vpx1000_mem_write8 = vpx1000_mem_uf_write8;
        vpx1000_mem_write16 = vpx1000_mem_uf_write16;
        vpx1000_mem_write32 = vpx1000_mem_uf_write32;

        vpx1000_mem_fetch8 = vpx1000_mem_uf_fetch8;
        vpx1000_mem_fetch16 = vpx1000_mem_uf_fetch16;
        vpx1000_mem_fetch32 = vpx1000_mem_uf_fetch32;

        vpx1000_mem_push8 = vpx1000_mem_uf_push8;
        vpx1000_mem_push16 = vpx1000_mem_uf_push16;
        vpx1000_mem_push32 = vpx1000_mem_uf_push32;

        vpx1000_mem_pop8 = vpx1000_mem_uf_pop8;
        vpx1000_mem_pop16 = vpx1000_mem_uf_pop16;
        vpx1000_mem_pop32 = vpx1000_mem_uf_pop32;

    }
}
void vpx1000_mem_setup(uint8_t* ptr, uint32_t size, uint8_t safety_mode){
    //[[ SETUP SAFTEY MODE ]]
    vpx1000_mem_safety_setup(safety_mode);
    //[[ SETUP SIZE AND PTR ]]
    vpx1000_mem_ptr = ptr;
    vpx1000_mem_size = size;
    //No saftey checks included because that's handled by the host.

}

//[[ ISA ]]

//ISA will be standard with the format:
//[opcode] [output reg] [operand reg] ...
//or
//[opcode] [address reg] [value reg] (STW r0 r1)
//Order applies to Immediate variants.















// === PRIMARY ISA / RISC ===


void vpx1000_isa_mov(){
    //Register move.
    uint8_t reg1 = vpx1000_mem_fetch8(); //Output register (By abiding to the format.)
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_cpu_wreg(reg1, val2);
}
void vpx1000_isa_movi(){
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    vpx1000_cpu_wreg(reg1, imm);

}

void vpx1000_isa_xchg(){
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);


    vpx1000_cpu_wreg(reg1, val2);
    vpx1000_cpu_wreg(reg2, val1);
    
}

void vpx1000_isa_ldb(){


    //=== get rpc value ===
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //We add the RPC value to transfer relative to objective values
    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;

    uint8_t val = vpx1000_mem_read8(adr);

    vpx1000_cpu_wreg(reg, val);
}
void vpx1000_isa_ldbi(){


    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t imm = vpx1000_mem_fetch8(); //Load immediate 8 bits  


    vpx1000_cpu_wreg(reg, imm);
}
void vpx1000_isa_aldb(){
    //Absolute addressing load byte.
    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //No addition as the value is absolute.
    uint32_t adr = vpx1000_cpu_rreg(adr_reg);

    uint8_t val = vpx1000_mem_read8(adr);

    vpx1000_cpu_wreg(reg, val);    
}

void vpx1000_isa_ldh(){


    //=== get rpc value ===
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //We add the RPC value to transfer relative to objective values
    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;

    uint16_t val = vpx1000_mem_read16(adr); //Half word (16 bit)

    vpx1000_cpu_wreg(reg, val);
}
void vpx1000_isa_ldhi(){


    uint8_t reg = vpx1000_mem_fetch8();
    uint16_t imm = vpx1000_mem_fetch16(); //Load immediate 16 bits  

    vpx1000_cpu_wreg(reg, imm);
}
void vpx1000_isa_aldh(){
    //Absolute addressing load half word.
    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //No addition as the value is absolute.
    uint32_t adr = vpx1000_cpu_rreg(adr_reg);

    uint16_t val = vpx1000_mem_read16(adr);

    vpx1000_cpu_wreg(reg, val);    
}

void vpx1000_isa_ldw(){


    //=== get rpc value ===
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //We add the RPC value to transfer relative to objective values
    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;

    uint32_t val = vpx1000_mem_read32(adr); //word (32 bit)

    vpx1000_cpu_wreg(reg, val);
}
void vpx1000_isa_ldwi(){
    //Literally just movi, So same thing i guess.
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    vpx1000_cpu_wreg(reg1, imm);

}
void vpx1000_isa_aldw(){
    //Absolute addressing load word.
    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();    



    //No addition as the value is absolute.
    uint32_t adr = vpx1000_cpu_rreg(adr_reg);

    uint32_t val = vpx1000_mem_read32(adr);

    vpx1000_cpu_wreg(reg, val);        
}

void vpx1000_isa_stb(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();


    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;
    uint8_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write8(adr, val);

}
void vpx1000_isa_astb(){

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();

    //Concidering it's absolute, No addition to RPC
    uint32_t adr = vpx1000_cpu_rreg(adr_reg);
    uint8_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write8(adr, val);

}
void vpx1000_isa_sth(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();


    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;
    uint16_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write16(adr, val);    
}
void vpx1000_isa_asth(){

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();

    uint32_t adr = vpx1000_cpu_rreg(adr_reg);
    uint16_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write16(adr, val);

}
void vpx1000_isa_stw(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();


    uint32_t adr = vpx1000_cpu_rreg(adr_reg) + RPC_val;
    uint32_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write32(adr, val);        
}
void vpx1000_isa_astw(){

    uint8_t reg = vpx1000_mem_fetch8();
    uint8_t adr_reg = vpx1000_mem_fetch8();

    uint32_t adr = vpx1000_cpu_rreg(adr_reg);
    uint32_t val = vpx1000_cpu_rreg(reg);

    vpx1000_mem_write32(adr, val);

}

void vpx1000_isa_jmp(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);

    uint8_t reg = vpx1000_mem_fetch8();

    uint32_t adr = vpx1000_cpu_rreg(reg) + RPC_val;

    vpx1000_cpu_wreg(vpx1000_RPC, adr);
}
void vpx1000_isa_ajmp(){
    //Absolute variant.

    uint8_t reg = vpx1000_mem_fetch8();

    uint32_t adr = vpx1000_cpu_rreg(reg);

    vpx1000_cpu_wreg(vpx1000_RPC, adr);
}

void vpx1000_isa_jmpi(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t adr = imm + RPC_val;

    vpx1000_cpu_wreg(vpx1000_RPC, adr);
}
void vpx1000_isa_ajmpi(){
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t adr = imm;

    vpx1000_cpu_wreg(vpx1000_RPC, adr);
}

void vpx1000_isa_jmpt(){
    if(vpx1000_f_condition_met){
        vpx1000_isa_jmp();
        return; //Exit early
    }
    vpx1000_mem_fetch8(); //Fetch to account for next operands.
}
void vpx1000_isa_ajmpt(){
    if(vpx1000_f_condition_met){
        vpx1000_isa_ajmp();
        return; //Exit early
    }
    vpx1000_mem_fetch8(); //Fetch to account for next operands.
}
void vpx1000_isa_jmpti(){
    if(vpx1000_f_condition_met){
        vpx1000_isa_jmpi();
        return; //Exit early
    }
    vpx1000_mem_fetch32(); //Fetch to account for next operands.
}
void vpx1000_isa_ajmpti(){
    if(vpx1000_f_condition_met){
        vpx1000_isa_ajmpi();
        return; //Exit early
    }
    vpx1000_mem_fetch32(); //Fetch to account for next operands.
}

void vpx1000_isa_jmpf(){
    if(!vpx1000_f_condition_met){
        vpx1000_isa_jmp();
        return; //Exit early
    }
    vpx1000_mem_fetch8(); //Fetch to account for next operands.
}
void vpx1000_isa_ajmpf(){
    if(!vpx1000_f_condition_met){
        vpx1000_isa_ajmp();
        return; //Exit early
    }
    vpx1000_mem_fetch8(); //Fetch to account for next operands.
}
void vpx1000_isa_jmpfi(){
    if(!vpx1000_f_condition_met){
        vpx1000_isa_jmpi();
        return; //Exit early
    }
    vpx1000_mem_fetch32(); //Fetch to account for next operands.
}
void vpx1000_isa_ajmpfi(){
    if(!vpx1000_f_condition_met){
        vpx1000_isa_ajmpi();
        return; //Exit early
    }
    vpx1000_mem_fetch32(); //Fetch to account for next operands.
}



void vpx1000_isa_cmpz(){
    //Check if register is zero.

    uint8_t reg = vpx1000_mem_fetch8();

    uint32_t val = vpx1000_cpu_rreg(reg);

    vpx1000_f_condition_met = 0; //Set to false by default.
    //If can't prove value is 0, then it'll stay false.

    if(!val){
        vpx1000_f_condition_met = 1; //True
    }

}
void vpx1000_isa_cmpg(){
    //Compare greater. reg1 > reg2


    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_f_condition_met = 0;

    if(val1 > val2){
        vpx1000_f_condition_met = 1; //True
    }


}
void vpx1000_isa_cmpge(){
    //Compare greater or equal. reg1 >= reg2
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();


    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_f_condition_met = 0;

    if(val1 >= val2){
        vpx1000_f_condition_met = 1; //True
    }


}
void vpx1000_isa_cmps(){
    //Compare smaller. reg1 < reg2

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();


    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_f_condition_met = 0;

    if(val1 < val2){
        vpx1000_f_condition_met = 1; //True
    }



}
void vpx1000_isa_cmpse(){
    //reg1 <= reg2

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();


    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_f_condition_met = 0;

    if(val1 <= val2){
        vpx1000_f_condition_met = 1; //True
    }



}
void vpx1000_isa_cmpe(){
    //reg1 == reg2;


    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();


    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    vpx1000_f_condition_met = 0;

    if(val1 == val2){
        vpx1000_f_condition_met = 1; //True
    }

}

void vpx1000_isa_cmpgi(){
    //Compare greater. reg1 > reg2

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = imm;

    vpx1000_f_condition_met = 0;

    if(val1 > val2){
        vpx1000_f_condition_met = 1; //True
    }


}
void vpx1000_isa_cmpgei(){

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = imm;

    vpx1000_f_condition_met = 0;

    if(val1 >= val2){
        vpx1000_f_condition_met = 1; //True
    }


}
void vpx1000_isa_cmpsi(){
    //Compare smaller. reg1 < reg2
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = imm;

    vpx1000_f_condition_met = 0;

    if(val1 < val2){
        vpx1000_f_condition_met = 1; //True
    }



}
void vpx1000_isa_cmpsei(){
    //reg1 <= reg2
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = imm;

    vpx1000_f_condition_met = 0;

    if(val1 <= val2){
        vpx1000_f_condition_met = 1; //True
    }



}
void vpx1000_isa_cmpei(){
    //reg1 == reg2;
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t imm = vpx1000_mem_fetch32();

    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    uint32_t val2 = imm;

    vpx1000_f_condition_met = 0;

    if(val1 == val2){
        vpx1000_f_condition_met = 1; //True
    }

}


void vpx1000_isa_pushb(){
    //reg1[0:8] -> stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    vpx1000_mem_push8(val1 & 0xff);

}
void vpx1000_isa_pushh(){
    //reg1[0:16] -> stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    vpx1000_mem_push16(val1 & 0xffff);

}
void vpx1000_isa_pushw(){
    //reg1[0:16] -> stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1);
    vpx1000_mem_push32(val1);

}

void vpx1000_isa_popb(){
    //reg1[0:8] <- stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_mem_pop8();
    vpx1000_cpu_wreg(reg1, val1);

}
void vpx1000_isa_poph(){
    //reg1[0:16] <- stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_mem_pop16();
    vpx1000_cpu_wreg(reg1, val1);

}
void vpx1000_isa_popw(){
    //reg1[0:32] <- stack[SP]

    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_mem_pop32();
    vpx1000_cpu_wreg(reg1, val1);

}

void vpx1000_isa_call(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);
    //Relative call
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1) + RPC_val;

    uint32_t current_adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_mem_push32(current_adr); //Store by pushing the current address

    //Write address to PC

    vpx1000_cpu_wreg(vpx1000_RPC, val1);

}
void vpx1000_isa_acall(){
    //Absolute call
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1);

    uint32_t current_adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_mem_push32(current_adr); //Store by pushing the current address

    //Write address to PC

    vpx1000_cpu_wreg(vpx1000_RPC, val1);

}
void vpx1000_isa_ret(){
    uint32_t return_adr = vpx1000_mem_pop32();

    vpx1000_cpu_wreg(vpx1000_RPC, return_adr);
    
}

void vpx1000_isa_inc(){
    //reg1++;
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1) + 1;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}
void vpx1000_isa_dec(){
    //reg1--;
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint32_t val1 = vpx1000_cpu_rreg(reg1) - 1;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}

void vpx1000_isa_add(){
    //reg1 = reg2 + reg3
    //addition
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 + val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_sub(){
    //reg1 = reg2 - reg3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 - val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_mul(){
    //reg1 = reg2 * reg3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 * val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_udiv(){
    //reg1 = reg2 / reg3
    //unsigned division
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 / val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_sdiv(){
    //reg1 = reg2 / reg3
    //signed division
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    int32_t val1 = (int32_t)val2 / (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}
void vpx1000_isa_urem(){
    //reg1 = reg2 % reg3
    //unsigned remainder
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 % val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_srem(){
    //reg1 = reg2 % reg3
    //signed remainder
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    int32_t val1 = (int32_t)val2 % (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}

void vpx1000_isa_not(){
    //r1 <- !r2
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val = vpx1000_cpu_rreg(reg2);
    val = ~val;

    vpx1000_cpu_wreg(reg1, val);
}
void vpx1000_isa_or(){
    //r1 = r2 or r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);

    uint32_t val1 = val2 | val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_xor(){
    //r1 = r2 xor r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 ^ val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_and(){
    //r1 = r2 or r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 & val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_shiftl(){
    //logical left shift
    //r1 = r2 << r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 << val3;

    vpx1000_cpu_wreg(reg1, val1);    
}
void vpx1000_isa_shiftr(){
    //logical right shift
    //r1 = r2 >> r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 >> val3;

    vpx1000_cpu_wreg(reg1, val1);    
}
void vpx1000_isa_shiftar(){
    //[[ WARNING, behaviour not guaranteed, avoid using this. ]]
    //arithmitic right shift
    //r1 = r2 arth>> r3
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    int32_t val1 = (int32_t)val2 >> (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);    
}


void vpx1000_isa_bit_set(){
    //r1 <- (r2[r3] = 1)
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 | (1u << (val3));

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_bit_clr(){
    //r1 <- (r2[r3] = 0)
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = val2 & ~(1u << (val3));

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_bit_test(){
    //condition met flag/reg1 = ?: reg2[reg3] == 1?
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t reg3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    uint32_t val3 = vpx1000_cpu_rreg(reg3);
    
    uint32_t val1 = (((val2) & (1u << (val3))) != 0);
    if(val1){
        vpx1000_f_condition_met = 1;
        val1 = 1;
    }else{
        vpx1000_f_condition_met = 0;
        val1 = 0;
    }

    vpx1000_cpu_wreg(reg1, val1);
}



void vpx1000_isa_calli(){
    uint32_t RPC_val = vpx1000_cpu_rreg(vpx1000_RPC);
    //Relative call
    uint32_t imm = vpx1000_mem_fetch32();
    uint32_t val1 = imm + RPC_val;

    uint32_t current_adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_mem_push32(current_adr); //Store by pushing the current address

    //Write address to PC

    vpx1000_cpu_wreg(vpx1000_RPC, val1);

}
void vpx1000_isa_acalli(){
    //Absolute call
    uint32_t imm = vpx1000_mem_fetch32();
    uint32_t val1 = imm;

    uint32_t current_adr = vpx1000_cpu_rreg(vpx1000_RPC);
    vpx1000_mem_push32(current_adr); //Store by pushing the current address

    //Write address to PC

    vpx1000_cpu_wreg(vpx1000_RPC, val1);

}

void vpx1000_isa_addi(){
    //reg1 = reg2 + imm
    //addition
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 + val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_subi(){
    //reg1 = reg2 - imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 - val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_muli(){
    //reg1 = reg2 * imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 * val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_udivi(){
    //reg1 = reg2 / imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 / val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_sdivi(){
    //reg1 = reg2 / imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    int32_t val1 = (int32_t)val2 / (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}
void vpx1000_isa_uremi(){
    //reg1 = reg2 % imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 % val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_sremi(){
    //reg1 = reg2 % imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    
    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    int32_t val1 = (int32_t)val2 % (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);
}

void vpx1000_isa_pushbi(){
    //val1[0:8] -> stack[SP]

    uint8_t val1 = vpx1000_mem_fetch8();
    vpx1000_mem_push8(val1);

}
void vpx1000_isa_pushhi(){
    //val1[0:16] -> stack[SP]

    uint16_t val1 = vpx1000_mem_fetch16();
    vpx1000_mem_push16(val1);

}
void vpx1000_isa_pushwi(){
    //val1[0:32] -> stack[SP]

    uint32_t val1 = vpx1000_mem_fetch32();
    vpx1000_mem_push32(val1);

}


void vpx1000_isa_ori(){
    //r1 = r2 or imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 | val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_xori(){
    //r1 = r2 xor imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 ^ val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_andi(){
    //r1 = r2 and imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint32_t val3 = vpx1000_mem_fetch32();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);

    uint32_t val1 = val2 & val3;

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_shiftli(){
    //logical left shift
    //r1 = r2 << imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    uint32_t val1 = val2 << val3;

    vpx1000_cpu_wreg(reg1, val1);    
}
void vpx1000_isa_shiftri(){
    //logical right shift
    //r1 = r2 >> imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    uint32_t val1 = val2 >> val3;

    vpx1000_cpu_wreg(reg1, val1);    
}
void vpx1000_isa_shiftari(){
    //[[ WARNING, behaviour not guaranteed, avoid using this. ]]
    //arithmitic right shift
    //r1 = r2 arth>> imm
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();
    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    int32_t val1 = (int32_t)val2 >> (int32_t)val3;

    vpx1000_cpu_wreg(reg1, (uint32_t)val1);    
}

void vpx1000_isa_bit_seti(){
    //r1 <- (r2[v3] = 1)
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    uint32_t val1 = val2 | (1u << (val3));

    vpx1000_cpu_wreg(reg1, val1);
}

void vpx1000_isa_bit_clri(){
    //r1 <- (r2[v3] = 0)
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    uint32_t val1 = val2 & ~(1u << (val3));

    vpx1000_cpu_wreg(reg1, val1);
}
void vpx1000_isa_bit_testi(){
    //condition met flag/reg1 = ?: reg2[imm] == 1?
    uint8_t reg1 = vpx1000_mem_fetch8();
    uint8_t reg2 = vpx1000_mem_fetch8();

    uint8_t val3 = vpx1000_mem_fetch8();

    uint32_t val2 = vpx1000_cpu_rreg(reg2);
    
    uint32_t val1 = (((val2) & (1u << (val3))) != 0);
    if(val1){
        vpx1000_f_condition_met = 1;
        val1 = 1;
    }else{
        vpx1000_f_condition_met = 0;
        val1 = 0;
    }

    vpx1000_cpu_wreg(reg1, val1);
}

//[[ EXEC ]]


uint8_t (*vpx1000_isa_exec)(void);

uint8_t vpx1000_isa_sf_exec(){
    //[[ SAFE EXECUTION VERSION ]]
    //Difference is this one returns an error and exits on invalid opcode.
    uint8_t opcode = vpx1000_mem_fetch8();

    switch(opcode){
        //Exit with code 255 (Opcode error!) and value of the opcode.
        default: vpx1000_f_error_code = 255; vpx1000_f_error_value = opcode; return 1; break;
        case 0: break; //Nop, no specified function.
        case 1: vpx1000_f_hostcall = 1; break; //Hostcall, Set variable

        case 2: vpx1000_isa_mov(); break;
        case 3: vpx1000_isa_xchg(); break;

        case 4: vpx1000_isa_ldb(); break;
        case 5: vpx1000_isa_ldh(); break;
        case 6: vpx1000_isa_ldw(); break;

        case 7: vpx1000_isa_aldb(); break;
        case 8: vpx1000_isa_aldh(); break;
        case 9: vpx1000_isa_aldw(); break;

        case 10: vpx1000_isa_stb(); break;
        case 11: vpx1000_isa_sth(); break;
        case 12: vpx1000_isa_stw(); break;

        case 13: vpx1000_isa_astb(); break;
        case 14: vpx1000_isa_asth(); break;
        case 15: vpx1000_isa_astw(); break;

        case 16: vpx1000_isa_jmp(); break;
        case 17: vpx1000_isa_ajmp(); break;

        

        case 18: vpx1000_isa_jmpt(); break;
        case 19: vpx1000_isa_ajmpt(); break;
        case 20: vpx1000_isa_jmpf(); break;
        case 21: vpx1000_isa_ajmpf(); break;

        case 22: vpx1000_isa_cmpz(); break;
        case 23: vpx1000_isa_cmpg(); break;
        case 24: vpx1000_isa_cmpge(); break;
        case 25: vpx1000_isa_cmps(); break;
        case 26: vpx1000_isa_cmpse(); break;
        case 27: vpx1000_isa_cmpe(); break;

        case 28: vpx1000_isa_pushb(); break;

        case 29: vpx1000_isa_pushh(); break;
        case 30: vpx1000_isa_pushw(); break;

        case 31: vpx1000_isa_popb(); break;
        case 32: vpx1000_isa_poph(); break;
        case 33: vpx1000_isa_popw(); break;

        case 34: vpx1000_isa_call(); break;
        case 35: vpx1000_isa_acall(); break;
        case 36: vpx1000_isa_ret(); break;

        case 37: vpx1000_isa_inc(); break;
        case 38: vpx1000_isa_dec(); break;

        case 39: vpx1000_isa_add(); break;
        case 40: vpx1000_isa_sub(); break;
        case 41: vpx1000_isa_mul(); break;
        case 42: vpx1000_isa_udiv(); break;
        case 43: vpx1000_isa_sdiv(); break;
        case 44: vpx1000_isa_urem(); break;
        case 45: vpx1000_isa_srem(); break;

        case 46: vpx1000_isa_not(); break;
        case 47: vpx1000_isa_or(); break;
        case 48: vpx1000_isa_xor(); break;
        case 49: vpx1000_isa_and(); break;

        case 50: vpx1000_isa_shiftl(); break;
        case 51: vpx1000_isa_shiftr(); break;
        case 52: vpx1000_isa_shiftar(); break;

        case 53: vpx1000_isa_bit_set(); break;
        case 54: vpx1000_isa_bit_clr(); break;
        case 55: vpx1000_isa_bit_test(); break;

        //[[ IMM VARIANTS ]]
        //Add more IMM instructions later for absolute variants.
        
       


        case 128 + 2: vpx1000_isa_movi(); break;

        case 128 + 4: vpx1000_isa_ldbi(); break;
        case 128 + 5: vpx1000_isa_ldhi(); break;
        case 128 + 6: vpx1000_isa_ldwi(); break;

        case 128 + 16: vpx1000_isa_jmpi(); break;
        case 128 + 17: vpx1000_isa_ajmpi(); break;

        case 128 + 18: vpx1000_isa_jmpti(); break;
        case 128 + 19: vpx1000_isa_ajmpti(); break;
        case 128 + 20: vpx1000_isa_jmpfi(); break;
        case 128 + 21: vpx1000_isa_ajmpfi(); break;

        case 128 + 23: vpx1000_isa_cmpgi(); break;
        case 128 + 24: vpx1000_isa_cmpgei(); break;
        case 128 + 25: vpx1000_isa_cmpsi(); break;
        case 128 + 26: vpx1000_isa_cmpsei(); break;
        case 128 + 27: vpx1000_isa_cmpei(); break;

        case 128 + 31: vpx1000_isa_pushbi(); break;
        case 128 + 32: vpx1000_isa_pushhi(); break;
        case 128 + 33: vpx1000_isa_pushwi(); break;

        case 128 + 34: vpx1000_isa_calli(); break;
        case 128 + 35: vpx1000_isa_acalli(); break;


        case 128 + 39: vpx1000_isa_addi(); break;
        case 128 + 40: vpx1000_isa_subi(); break;
        case 128 + 41: vpx1000_isa_muli(); break;
        case 128 + 42: vpx1000_isa_udivi(); break;
        case 128 + 43: vpx1000_isa_sdivi(); break;
        case 128 + 44: vpx1000_isa_uremi(); break;
        case 128 + 45: vpx1000_isa_sremi(); break;

        case 128 + 47: vpx1000_isa_ori(); break;
        case 128 + 48: vpx1000_isa_xori(); break;
        case 128 + 49: vpx1000_isa_andi(); break;

        case 128 + 50: vpx1000_isa_shiftli(); break;
        case 128 + 51: vpx1000_isa_shiftri(); break;
        case 128 + 52: vpx1000_isa_shiftari(); break;

        case 128 + 53: vpx1000_isa_bit_seti(); break;
        case 128 + 54: vpx1000_isa_bit_clri(); break;
        case 128 + 55: vpx1000_isa_bit_testi(); break;


    }

    return opcode;

}
uint8_t vpx1000_isa_uf_exec(){
    //[[ UNSAFE EXECUTION VERSION ]]
    //Difference is this one treats invalid opcodes as NOP!
    uint8_t opcode = vpx1000_mem_fetch8();

    switch(opcode){
        //Don't exit and treat as nop!
        default: break;
        case 0: break; //Nop, no specified function.
        case 1: vpx1000_f_hostcall = 1; break; //Hostcall, Set variable

        case 2: vpx1000_isa_mov(); break;
        case 3: vpx1000_isa_xchg(); break;

        case 4: vpx1000_isa_ldb(); break;
        case 5: vpx1000_isa_ldh(); break;
        case 6: vpx1000_isa_ldw(); break;

        case 7: vpx1000_isa_aldb(); break;
        case 8: vpx1000_isa_aldh(); break;
        case 9: vpx1000_isa_aldw(); break;

        case 10: vpx1000_isa_stb(); break;
        case 11: vpx1000_isa_sth(); break;
        case 12: vpx1000_isa_stw(); break;

        case 13: vpx1000_isa_astb(); break;
        case 14: vpx1000_isa_asth(); break;
        case 15: vpx1000_isa_astw(); break;

        case 16: vpx1000_isa_jmp(); break;
        case 17: vpx1000_isa_ajmp(); break;

        

        case 18: vpx1000_isa_jmpt(); break;
        case 19: vpx1000_isa_ajmpt(); break;
        case 20: vpx1000_isa_jmpf(); break;
        case 21: vpx1000_isa_ajmpf(); break;

        case 22: vpx1000_isa_cmpz(); break;
        case 23: vpx1000_isa_cmpg(); break;
        case 24: vpx1000_isa_cmpge(); break;
        case 25: vpx1000_isa_cmps(); break;
        case 26: vpx1000_isa_cmpse(); break;
        case 27: vpx1000_isa_cmpe(); break;

        case 28: vpx1000_isa_pushb(); break;

        case 29: vpx1000_isa_pushh(); break;
        case 30: vpx1000_isa_pushw(); break;

        case 31: vpx1000_isa_popb(); break;
        case 32: vpx1000_isa_poph(); break;
        case 33: vpx1000_isa_popw(); break;

        case 34: vpx1000_isa_call(); break;
        case 35: vpx1000_isa_acall(); break;
        case 36: vpx1000_isa_ret(); break;

        case 37: vpx1000_isa_inc(); break;
        case 38: vpx1000_isa_dec(); break;

        case 39: vpx1000_isa_add(); break;
        case 40: vpx1000_isa_sub(); break;
        case 41: vpx1000_isa_mul(); break;
        case 42: vpx1000_isa_udiv(); break;
        case 43: vpx1000_isa_sdiv(); break;
        case 44: vpx1000_isa_urem(); break;
        case 45: vpx1000_isa_srem(); break;

        case 46: vpx1000_isa_not(); break;
        case 47: vpx1000_isa_or(); break;
        case 48: vpx1000_isa_xor(); break;
        case 49: vpx1000_isa_and(); break;

        case 50: vpx1000_isa_shiftl(); break;
        case 51: vpx1000_isa_shiftr(); break;
        case 52: vpx1000_isa_shiftar(); break;

        case 53: vpx1000_isa_bit_set(); break;
        case 54: vpx1000_isa_bit_clr(); break;
        case 55: vpx1000_isa_bit_test(); break;

        //[[ IMM VARIANTS ]]
        //Add more IMM instructions later for absolute variants.
        
       


        case 128 + 2: vpx1000_isa_movi(); break;

        case 128 + 4: vpx1000_isa_ldbi(); break;
        case 128 + 5: vpx1000_isa_ldhi(); break;
        case 128 + 6: vpx1000_isa_ldwi(); break;

        case 128 + 16: vpx1000_isa_jmpi(); break;
        case 128 + 17: vpx1000_isa_ajmpi(); break;

        case 128 + 18: vpx1000_isa_jmpti(); break;
        case 128 + 19: vpx1000_isa_ajmpti(); break;
        case 128 + 20: vpx1000_isa_jmpfi(); break;
        case 128 + 21: vpx1000_isa_ajmpfi(); break;

        case 128 + 23: vpx1000_isa_cmpgi(); break;
        case 128 + 24: vpx1000_isa_cmpgei(); break;
        case 128 + 25: vpx1000_isa_cmpsi(); break;
        case 128 + 26: vpx1000_isa_cmpsei(); break;
        case 128 + 27: vpx1000_isa_cmpei(); break;

        case 128 + 31: vpx1000_isa_pushbi(); break;
        case 128 + 32: vpx1000_isa_pushhi(); break;
        case 128 + 33: vpx1000_isa_pushwi(); break;

        case 128 + 34: vpx1000_isa_calli(); break;
        case 128 + 35: vpx1000_isa_acalli(); break;


        case 128 + 39: vpx1000_isa_addi(); break;
        case 128 + 40: vpx1000_isa_subi(); break;
        case 128 + 41: vpx1000_isa_muli(); break;
        case 128 + 42: vpx1000_isa_udivi(); break;
        case 128 + 43: vpx1000_isa_sdivi(); break;
        case 128 + 44: vpx1000_isa_uremi(); break;
        case 128 + 45: vpx1000_isa_sremi(); break;

        case 128 + 47: vpx1000_isa_ori(); break;
        case 128 + 48: vpx1000_isa_xori(); break;
        case 128 + 49: vpx1000_isa_andi(); break;

        case 128 + 50: vpx1000_isa_shiftli(); break;
        case 128 + 51: vpx1000_isa_shiftri(); break;
        case 128 + 52: vpx1000_isa_shiftari(); break;

        case 128 + 53: vpx1000_isa_bit_seti(); break;
        case 128 + 54: vpx1000_isa_bit_clri(); break;
        case 128 + 55: vpx1000_isa_bit_testi(); break;

    }

    return opcode;
}

void vpx1000_isa_setup(uint8_t saftey_mode){
    if(saftey_mode){
        vpx1000_isa_exec = vpx1000_isa_sf_exec; //Safe mode

    }
    else{
        vpx1000_isa_exec = vpx1000_isa_uf_exec; //Unsafe mode
    }
}

//[[ PRIMARY ]]


void vpx1000_init(uint8_t* ptr, uint32_t size, uint8_t mode){
    //Full setup for all vpx1000 peripherals.
    vpx1000_mem_setup(ptr, size, mode);
    vpx1000_isa_setup(mode);
    vpx1000_f_hostcall = 0;
    vpx1000_f_condition_met = 0;
}
uint8_t vpx1000_step(){
    return vpx1000_isa_exec();
}
uint8_t vpx1000_start(){

    while(1){
        uint8_t rt = vpx1000_step();

        //[[ HOST-CALL CHECK ]]
        if(vpx1000_f_hostcall){ return 0; }

        //[[ ERROR CHECK ]]
        if(vpx1000_f_error_code){ return 1; } //Error

    }
    return 0;
}
