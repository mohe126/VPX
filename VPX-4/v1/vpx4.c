//[[ PRIMARY INCLUDE ]]
#include "vpx4.h"
#include "vpx4-isa.h"




//[[ CONFIG FUNCTIONS ]]
void vpx4_config_isa(Vpx4* vpx4, uint8_t opcode, vpx4_instruction_t ins){
    //In the init function, The defaults will be set.
    //Please do not change unless you are willing to sacrifice portability
    //This is for debug purposes

    //Most reasonable use cases are IO functions (If not wanting to use hostcalls)
    //Or for debugging/64 bit extensions, Using it for other than that is not recommended.
    //Unless you really know what you're doing.

    vpx4->isa[opcode] = ins;

}
void vpx4_config_reg(Vpx4* vpx4, uint32_t* ptr, uint8_t count){
    //By default, 64 registers. (similar to vpx4)
    //Please do not change unless you are willing to sacrifice portability
    //This is for debug purposes
    vpx4->registers.ptr = ptr;
    vpx4->registers.count = count;

}
void vpx4_config_mem(Vpx4* vpx4, uint8_t* ptr, uint32_t alloc){
    vpx4->mem.ptr = ptr;
    vpx4->mem.alloc = alloc;
}



//[[ ERROR FUNCTIONS ]]
void vpx4_err(Vpx4* vpx4, uint8_t code, uint32_t value){
    vpx4->error.code = code;
    vpx4->error.value = value;
}





//[[ REGISTER FUNCTIONS ]]
uint32_t vpx4_rreg(Vpx4* vpx4, uint8_t r){
    if(r >= vpx4->registers.count){
        vpx4_err(vpx4, VPX4_E_RREG, r);
        return 0;
    }

    return vpx4->registers.ptr[r];
}
void vpx4_wreg(Vpx4* vpx4, uint8_t r, uint32_t val){
    if(r >= vpx4->registers.count){
        vpx4_err(vpx4, VPX4_E_WREG, r);
        return;
    }

    vpx4->registers.ptr[r] = val;
}
//[[ MEMORY FUNCTIONS ]]
void vpx4_mem_w08(Vpx4* vpx4, uint32_t adr, uint8_t val){
    if(adr >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MW08, adr);
        return;
    }

    vpx4->mem.ptr[adr] = val;
}
void vpx4_mem_w16(Vpx4* vpx4, uint32_t adr, uint16_t val){
    if(adr + 1 >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MW16, adr);
        return;
    }

    uint16_t tmp = vpx4_endian_fmt16(val);
    memcpy(&vpx4->mem.ptr[adr], &tmp, sizeof(tmp));
}
void vpx4_mem_w32(Vpx4* vpx4, uint32_t adr, uint32_t val){
    if(adr + 32 >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MW32, adr);
        return;
    }

    uint32_t tmp = vpx4_endian_fmt32(val);
    memcpy(&vpx4->mem.ptr[adr], &tmp, sizeof(tmp));
}

uint8_t vpx4_mem_r08(Vpx4* vpx4, uint32_t adr){
    if(adr >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MR08, adr);
        return 0;
    }

    return vpx4->mem.ptr[adr];
}
uint16_t vpx4_mem_r16(Vpx4* vpx4, uint32_t adr){
    if(adr + 1 >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MR16, adr);
        return 0;
    }

    uint16_t tmp;
    memcpy(&vpx4->mem.ptr[adr], &tmp, sizeof(tmp));

    return vpx4_endian_fmt16(tmp);
}
uint32_t vpx4_mem_r32(Vpx4* vpx4, uint32_t adr){
    if(adr + 3 >= vpx4->mem.alloc){
        vpx4_err(vpx4, VPX4_E_MR32, adr);
        return 0;
    }

    uint32_t tmp;
    memcpy(&vpx4->mem.ptr[adr], &tmp, sizeof(tmp));

    return vpx4_endian_fmt32(tmp);
}

void vpx4_mem_pu08(Vpx4* vpx4, uint8_t val){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_mem_w08(vpx4, adr, val);

    vpx4_wreg(vpx4, VPX4_RSP, adr+1);
}
void vpx4_mem_pu16(Vpx4* vpx4, uint16_t val){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_mem_w16(vpx4, adr, val);

    vpx4_wreg(vpx4, VPX4_RSP, adr+2);
}
void vpx4_mem_pu32(Vpx4* vpx4, uint32_t val){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_mem_w32(vpx4, adr, val);

    vpx4_wreg(vpx4, VPX4_RSP, adr+4);
}

uint8_t vpx4_mem_po08(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_wreg(vpx4, VPX4_RSP, adr-1);

    return vpx4_mem_r08(vpx4, adr-1);

}
uint16_t vpx4_mem_po16(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_wreg(vpx4, VPX4_RSP, adr-2);

    return vpx4_mem_r16(vpx4, adr-2);

}
uint32_t vpx4_mem_po32(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RSP);

    vpx4_wreg(vpx4, VPX4_RSP, adr-4);

    return vpx4_mem_r32(vpx4, adr-4);
}

uint8_t vpx4_mem_f08(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RPC);


    uint8_t val = vpx4_mem_r08(vpx4, adr);


    vpx4_wreg(vpx4, VPX4_RPC, adr+1);


    return val;
}
uint16_t vpx4_mem_f16(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RPC);


    uint16_t val = vpx4_mem_r16(vpx4, adr);


    vpx4_wreg(vpx4, VPX4_RPC, adr+2);


    return val;
}
uint32_t vpx4_mem_f32(Vpx4* vpx4){
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RPC);


    uint32_t val = vpx4_mem_r32(vpx4, adr);


    vpx4_wreg(vpx4, VPX4_RPC, adr+4);


    return val;
}


//[[ SETUP FUNCTIONS ]]

uint8_t vpx4_exec(Vpx4* vpx4){

    uint8_t opcode = vpx4_mem_f08(vpx4);
    vpx4->isa[opcode](vpx4);

    if(vpx4->hostcall){
        return 2; //Exit with success.
    }
    if(vpx4->error.code){
        return 1; //Exit with failure/exception.
    }

    return 0; //No HC or Error.


}
void vpx4_init(Vpx4* vpx4, uint8_t* mem_ptr, uint32_t mem_alloc){
    //[[ REGISTERS ]]

    vpx4->registers.ptr = vpx4->default_registers;
    vpx4->registers.count = 64;

    vpx4_wreg(vpx4, VPX4_RPC, 0);


    //[[ EXCEPTION/ERROR ]]

    vpx4->error.code = 0;
    vpx4->error.value = 0;

    //[[ MEMORY ]]

    vpx4->mem.ptr = mem_ptr;
    vpx4->mem.alloc = mem_alloc;

    //[[ HOSTCALL ]]

    vpx4->hostcall = 0;


    //[[ ISA ]]

    //== INITIAL EXCEPTION FILL ==
    for(uint32_t i = 0; i < 256; i++){
        vpx4->isa[i] = vpx4_isa_except;
    }

    //== INDIVIDUAL/SPECIFIC ==

    vpx4->isa[0] = vpx4_isa_nop;
    vpx4->isa[1] = vpx4_isa_hostcall;
    vpx4->isa[2] = vpx4_isa_mov;

    vpx4->isa[3] = vpx4_isa_movi;

    vpx4->isa[4] = vpx4_isa_inc;
    vpx4->isa[5] = vpx4_isa_dec;

    vpx4->isa[6] = vpx4_isa_or;
    vpx4->isa[7] = vpx4_isa_xor;
    vpx4->isa[8] = vpx4_isa_and;
    vpx4->isa[9] = vpx4_isa_not;

    vpx4->isa[10] = vpx4_isa_ori;
    vpx4->isa[11] = vpx4_isa_xori;
    vpx4->isa[12] = vpx4_isa_andi;

    vpx4->isa[13] = vpx4_isa_sll;
    vpx4->isa[14] = vpx4_isa_srl;
    vpx4->isa[15] = vpx4_isa_sra;

    vpx4->isa[16] = vpx4_isa_slli;
    vpx4->isa[17] = vpx4_isa_srli;
    vpx4->isa[18] = vpx4_isa_srai;

    vpx4->isa[19] = vpx4_isa_add;
    vpx4->isa[20] = vpx4_isa_sub;
    vpx4->isa[21] = vpx4_isa_mul;
    vpx4->isa[22] = vpx4_isa_udiv;
    vpx4->isa[23] = vpx4_isa_sdiv;
    vpx4->isa[24] = vpx4_isa_urem;
    vpx4->isa[25] = vpx4_isa_srem;

    vpx4->isa[26] = vpx4_isa_addi;
    vpx4->isa[27] = vpx4_isa_subi;
    vpx4->isa[28] = vpx4_isa_muli;
    vpx4->isa[29] = vpx4_isa_udivi;
    vpx4->isa[30] = vpx4_isa_sdivi;
    vpx4->isa[31] = vpx4_isa_uremi;
    vpx4->isa[32] = vpx4_isa_sremi;

    vpx4->isa[33] = vpx4_isa_ld8i;
    vpx4->isa[34] = vpx4_isa_ld16i;
    vpx4->isa[35] = vpx4_isa_ld32i;
    vpx4->isa[36] = vpx4_isa_ld8r;
    vpx4->isa[37] = vpx4_isa_ld16r;
    vpx4->isa[38] = vpx4_isa_ld32r;

    vpx4->isa[39] = vpx4_isa_st8i;
    vpx4->isa[40] = vpx4_isa_st16i;
    vpx4->isa[41] = vpx4_isa_st32i;
    vpx4->isa[42] = vpx4_isa_st8r;
    vpx4->isa[43] = vpx4_isa_st16r;
    vpx4->isa[44] = vpx4_isa_st32r;

    vpx4->isa[45] = vpx4_isa_jmpi;
    vpx4->isa[46] = vpx4_isa_jmpr;

    vpx4->isa[47] = vpx4_isa_zjmpi;
    vpx4->isa[48] = vpx4_isa_eqjmpi;
    vpx4->isa[49] = vpx4_isa_nejmpi;
    vpx4->isa[50] = vpx4_isa_gjmpi;
    vpx4->isa[51] = vpx4_isa_gejmpi;

    vpx4->isa[52] = vpx4_isa_push8;
    vpx4->isa[53] = vpx4_isa_push16;
    vpx4->isa[54] = vpx4_isa_push32;

    vpx4->isa[55] = vpx4_isa_pop8;
    vpx4->isa[56] = vpx4_isa_pop16;
    vpx4->isa[57] = vpx4_isa_pop32;

    vpx4->isa[58] = vpx4_isa_calli;
    vpx4->isa[59] = vpx4_isa_callr;
    vpx4->isa[60] = vpx4_isa_ret;




    

}