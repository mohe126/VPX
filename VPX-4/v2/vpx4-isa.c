
#include "vpx4.h"
//[[ ISA ]]

//[[ BASIC/CONTROL INSTRUCTIONS ]]
void vpx4_isa_except(Vpx4* vpx4){
    //By default gets the opcode i guess
    uint32_t adr = vpx4_rreg(vpx4, VPX4_RPC);
    uint8_t opcode = vpx4_mem_r08(vpx4, adr-1);
    vpx4_err(vpx4, VPX4_E_INVOP, opcode);

}
void vpx4_isa_nop(Vpx4* vpx4){
    return;
}
void vpx4_isa_hostcall(Vpx4* vpx4){
    vpx4->hostcall = 1;
    return;
}



//[[ VPX3 COMPATIBLE ]]
//[[ NOTE: ]]
//v2 or higher might have a different ISA generally.
//A newer custom made one.




void vpx4_isa_mov(Vpx4* vpx4){
    //===========================================
    //Move value in r1 to r2.
    //===========================================
    //C syntax: registers[r1] = registers[r2];
    //Pseudocode: r1 <- r2
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t val = vpx4_rreg(vpx4, r2);
    vpx4_wreg(vpx4, r1, val);
    

}
void vpx4_isa_movi(Vpx4* vpx4){
    //===========================================
    //Move immediate value to r1
    //===========================================
    //C syntax: registers[r1] = imm;
    //Pseudocode: r1 <- imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);
    vpx4_wreg(vpx4, r1, imm);
}
void vpx4_isa_inc(Vpx4* vpx4){
    //===========================================
    //Increment value of r1
    //===========================================
    //C syntax: registers[r1] = registers[r1]++;
    //Pseudocode: r1 <- r1 + 1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val = vpx4_rreg(vpx4, r1);
    vpx4_wreg(vpx4, r1, val + 1);
}
void vpx4_isa_dec(Vpx4* vpx4){
    //===========================================
    //Decrement value of r1
    //===========================================
    //C syntax: registers[r1] = registers[r1]--;
    //Pseudocode: r1 <- r1 - 1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val = vpx4_rreg(vpx4, r1);
    vpx4_wreg(vpx4, r1, val - 1);
}

void vpx4_isa_or(Vpx4* vpx4){
    //===========================================
    //Do an OR operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] | registers[r3];
    //Pseudocode: r1 <- r2 or r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 | val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_xor(Vpx4* vpx4){
    //===========================================
    //Do an XOR operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] ^ registers[r3];
    //Pseudocode: r1 <- r2 xor r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 ^ val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_and(Vpx4* vpx4){
    //===========================================
    //Do an AND operation on r2 and r3, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] & registers[r3];
    //Pseudocode: r1 <- r2 and r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 & val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_not(Vpx4* vpx4){
    //===========================================
    //Do a NOT operation on r2 and write to r1
    //===========================================
    //C syntax: registers[r1] = ~registers[r2];
    //Pseudocode: r1 <- not r2
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = ~val2;
    vpx4_wreg(vpx4, r1, val1);
}

void vpx4_isa_ori(Vpx4* vpx4){
    //===========================================
    //Do an OR operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] | imm;
    //Pseudocode: r1 <- r2 or imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 | imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_xori(Vpx4* vpx4){
    //===========================================
    //Do an XOR operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] ^ imm;
    //Pseudocode: r1 <- r2 xor imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 ^ imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_andi(Vpx4* vpx4){
    //===========================================
    //Do an AND operation on r2 and immediate, write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] & imm;
    //Pseudocode: r1 <- r2 and imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 & imm;
    vpx4_wreg(vpx4, r1, val1);
}

void vpx4_isa_sll(Vpx4* vpx4){
    //===========================================
    //Shift logical left of r2 by r3 and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] << registers[r3];
    //Pseudocode: r1 <- r2 << r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 << val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_srl(Vpx4* vpx4){
    //===========================================
    //Shift logical right of r2 by r3 and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> registers[r3];
    //Pseudocode: r1 <- r2 >> r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 >> val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_sra(Vpx4* vpx4){
    //===========================================
    //Shift arithmetic right of r2 by r3 and write to r1
    //WARNING: might not work always!
    //might add inline assembly version if this doesn't work. with preproccessor checks
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> registers[r3];
    //Pseudocode: r1 <- r2 >> r3
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    int32_t val1 = (int32_t)val2 >> (int32_t)val3;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}

void vpx4_isa_slli(Vpx4* vpx4){
    //===========================================
    //Immediate Shift logical left of r2 by imm and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] << imm;
    //Pseudocode: r1 <- r2 << imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t imm = vpx4_mem_f08(vpx4); //imm is 8 bits because you can't shift by more anyway

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 << imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_srli(Vpx4* vpx4){
    //===========================================
    //Immediate Shift logical right of r2 by imm and write to r1
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> imm;
    //Pseudocode: r1 <- r2 >> imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t imm = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
 

    uint32_t val1 = val2 >> imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_srai(Vpx4* vpx4){
    //===========================================
    //Immediate Shift arithmetic right of r2 by imm and write to r1
    //WARNING: might not work always!
    //might add inline assembly version if this doesn't work. with preproccessor checks
    //===========================================
    //C syntax: registers[r1] = registers[r2] >> imm;
    //Pseudocode: r1 <- r2 >> imm
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t imm = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    int32_t val1 = (int32_t)val2 >> (int32_t)imm;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}

void vpx4_isa_add(Vpx4* vpx4){
    //===========================================
    //Add r2 and r3, write result to r1. (No carry)
    //===========================================
    //C syntax: registers[r1] = registers[r2] + registers[r3];
    //Pseudocode: r1 <- r2 + r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 + val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_sub(Vpx4* vpx4){
    //===========================================
    //Subtract r2 by r3, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] - registers[r3];
    //Pseudocode: r1 <- r2 - r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 - val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_mul(Vpx4* vpx4){
    //===========================================
    //Multiply r2 by r3, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] * registers[r3];
    //Pseudocode: r1 <- r2 * r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t val1 = val2 * val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_udiv(Vpx4* vpx4){
    //===========================================
    //Divide r2 by r3, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / registers[r3];
    //Pseudocode: r1 <- r2 / r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);
    if(val3 == 0){
        //Division by 0 error
        //Log aswell the register that contained it.
        vpx4_err(vpx4, VPX4_E_DB0, r3);
        return;
    }

    uint32_t val1 = val2 / val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_sdiv(Vpx4* vpx4){
    //===========================================
    //Divide r2 by r3, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / registers[r3];
    //Pseudocode: r1 <- r2 / r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);
    //Signed version is stricter.
    if((val3 == 0) || ((int32_t)val2 == UINT32_MAX && (int32_t)val2 == -1)){
        //Division by 0 error (But signed)
        //Log aswell the register that contained it.
        vpx4_err(vpx4, VPX4_E_DB0S, r3);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)val3 == -1){
        vpx4_err(vpx4, VPX4_E_DI32M, r2); //Signed conversion error.
        return;
    }


    int32_t val1 = (int32_t)val2 / (int32_t)val3;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}
void vpx4_isa_urem(Vpx4* vpx4){
    //===========================================
    //Modulo/Remainder of r2 by r3, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % registers[r3];
    //Pseudocode: r1 <- r2 % r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);
    if(val3 == 0){
        //Division by 0 error
        //Log aswell the register that contained it.
        vpx4_err(vpx4, 9, r3);
        return;
    }

    uint32_t val1 = val2 % val3;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_srem(Vpx4* vpx4){
    //===========================================
    //Modulo/Remainder of r2 by r3, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % registers[r3];
    //Pseudocode: r1 <- r2 % r3
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    //Signed version is stricter.
    if(val3 == 0){
        //Division by 0 error (But signed)
        //Log aswell the register that contained it.
        vpx4_err(vpx4, 10, r3);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)val3 == -1){
        vpx4_err(vpx4, 11, r2);
        return;
    }


    int32_t val1 = (int32_t)val2 % (int32_t)val3;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}

void vpx4_isa_addi(Vpx4* vpx4){
    //===========================================
    //Add r2 and imm, write result to r1. (No carry)
    //===========================================
    //C syntax: registers[r1] = registers[r2] + imm;
    //Pseudocode: r1 <- r2 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 + imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_subi(Vpx4* vpx4){
    //===========================================
    //Subtract r2 and imm, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] - imm;
    //Pseudocode: r1 <- r2 - imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 - imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_muli(Vpx4* vpx4){
    //===========================================
    //Multiply r2 by imm, write result to r1.
    //===========================================
    //C syntax: registers[r1] = registers[r2] * imm;
    //Pseudocode: r1 <- r2 * imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t val1 = val2 * imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_udivi(Vpx4* vpx4){
    //===========================================
    //Divide r2 by imm, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / imm;
    //Pseudocode: r1 <- r2 / imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    if(imm == 0){
        //Division by 0 error
        vpx4_err(vpx4, VPX4_E_DB0, 0);
        return;
    }

    uint32_t val1 = val2 / imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_sdivi(Vpx4* vpx4){
    //===========================================
    //Divide r2 by imm, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] / imm;
    //Pseudocode: r1 <- r2 / imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    //Signed version is stricter.
    if(imm == 0){
        //Division by 0 error (But signed)
        vpx4_err(vpx4, VPX4_E_DB0S, 0);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)imm == -1){
        vpx4_err(vpx4, VPX4_E_DI32M, r2); //Signed conversion error.
        return;
    }


    int32_t val1 = (int32_t)val2 / (int32_t)imm;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}
void vpx4_isa_uremi(Vpx4* vpx4){
    //===========================================
    //Modulo/Remainder of r2 by imm, write result to r1. (Unsigned)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % imm;
    //Pseudocode: r1 <- r2 % imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    if(imm == 0){
        //Division by 0 error
        vpx4_err(vpx4, 9, 0);
        return;
    }

    uint32_t val1 = val2 % imm;
    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_sremi(Vpx4* vpx4){
    //===========================================
    //Modulo/Remainder of r2 by imm, write result to r1. (Signed)
    //===========================================
    //C syntax: registers[r1] = registers[r2] % imm;
    //Pseudocode: r1 <- r2 % imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    //Signed version is stricter.
    if(imm == 0){
        //Division by 0 error (But signed)
        vpx4_err(vpx4, 10, 0);
        return;
    }
    if((int32_t)val2 == INT32_MIN && (int32_t)imm == -1){
        vpx4_err(vpx4, 11, r2);
        return;
    }


    int32_t val1 = (int32_t)val2 % (int32_t)imm;
    vpx4_wreg(vpx4, r1, (uint32_t)val1);
}


//=IMMEDIATE=
void vpx4_isa_ld8i(Vpx4* vpx4){
    //all these instructions, even if have the same aliases as from older versions, ARE different.
    //===========================================
    //Read 8 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r8(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    uint8_t val1 = vpx4_mem_r08(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}
void vpx4_isa_ld16i(Vpx4* vpx4){
    //===========================================
    //Read 16 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r16(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    uint16_t val1 = vpx4_mem_r16(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}
void vpx4_isa_ld32i(Vpx4* vpx4){
    //===========================================
    //Read 32 bits from address defined by: (r2 + imm) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r32(registers[r2] + imm);
    //Pseudocode: r1 <- mem[r2 + imm]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    uint32_t val1 = vpx4_mem_r32(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}
void vpx4_isa_ld8r(Vpx4* vpx4){
    //===========================================
    //Read 8 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r8(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);


    uint32_t adr = val2 + val3;

    uint8_t val1 = vpx4_mem_r08(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}
void vpx4_isa_ld16r(Vpx4* vpx4){
    //===========================================
    //Read 16 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r16(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);


    uint32_t adr = val2 + val3;

    uint16_t val1 = vpx4_mem_r16(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}
void vpx4_isa_ld32r(Vpx4* vpx4){
    //===========================================
    //Read 32 bits from address defined by: (r2 + r3) to r1.
    //===========================================
    //C syntax: registers[r1] = mem_r32(registers[r2] + registers[r3]);
    //Pseudocode: r1 <- mem[r2 + r3]
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);


    uint32_t adr = val2 + val3;

    uint32_t val1 = vpx4_mem_r32(vpx4, adr);
    vpx4_wreg(vpx4, r1, val1);

}

void vpx4_isa_st8i(Vpx4* vpx4){
    //all these instructions, even if have the same aliases as from older versions, ARE different.
    //===========================================
    //Store 8 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w8(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    vpx4_mem_w08(vpx4, adr, val1 & 0xFF);
}
void vpx4_isa_st16i(Vpx4* vpx4){
    //===========================================
    //Store 16 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w16(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    vpx4_mem_w16(vpx4, adr, val1 & 0xFFFF);
}
void vpx4_isa_st32i(Vpx4* vpx4){
    //===========================================
    //Store 32 bits from r1 to address defined by: (r2 + imm)
    //===========================================
    //C syntax: mem_w32(registers[r2] + imm, registers[r1]);
    //Pseudocode: mem[r2 + imm] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t adr = val2 + imm;

    vpx4_mem_w32(vpx4, adr, val1);
}
void vpx4_isa_st8r(Vpx4* vpx4){
    //===========================================
    //Store 8 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w8(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t adr = val2 + val3;

    vpx4_mem_w08(vpx4, adr, val1 & 0xFF);
}
void vpx4_isa_st16r(Vpx4* vpx4){
    //===========================================
    //Store 16 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w16(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t adr = val2 + val3;

    vpx4_mem_w16(vpx4, adr, val1 & 0xFFFF);
}
void vpx4_isa_st32r(Vpx4* vpx4){
    //===========================================
    //Store 32 bits from r1 to address defined by: (r2 + r3)
    //===========================================
    //C syntax: mem_w32(registers[r2] + registers[r3], registers[r1]);
    //Pseudocode: mem[r2 + r3] = r1
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t val3 = vpx4_rreg(vpx4, r3);

    uint32_t adr = val2 + val3;

    vpx4_mem_w32(vpx4, adr, val1);
}

void vpx4_isa_jmpi(Vpx4* vpx4){
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
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);

    uint32_t valrpc = val1 + imm;
    vpx4_wreg(vpx4, VPX4_RPC, valrpc);
}
void vpx4_isa_jmpr(Vpx4* vpx4){

    //this instruction can be replaced with: add, rpc, r1, r2, But it's prefferable to use this
    //For the sake of simplicity!
    //===========================================
    //Jump to address defined by: PC = (r1 + r2)
    //===========================================
    //C syntax: registers[rpc] = registers[r1] + registers[r2];
    //Pseudocode: rpc = r1 + r2
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);

    uint32_t valrpc = val1 + val2;
    vpx4_wreg(vpx4, VPX4_RPC, valrpc);
}

void vpx4_isa_zjmpi(Vpx4* vpx4){
    //===========================================
    //Conditional Jump to address defined by: PC = (r2 + imm) IF: r1 == 0
    //===========================================
    //C syntax: if(registers[r1] == 0){registers[rpc] = registers[r2] + imm;}
    //Pseudocode: if r1 = 0: rpc = r2 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);

    if(val1 == 0){
        uint32_t val2 = vpx4_rreg(vpx4, r2);
        uint32_t valrpc = val2 + imm;
        vpx4_wreg(vpx4, VPX4_RPC, valrpc);
    }

}
void vpx4_isa_eqjmpi(Vpx4* vpx4){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 = r2
    //===========================================
    //C syntax: if(registers[r1] == registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 = r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    
    if(val1 == val2){
        uint32_t val3 = vpx4_rreg(vpx4, r3);
        uint32_t valrpc = val3 + imm;
        vpx4_wreg(vpx4, VPX4_RPC, valrpc);
    }

}
void vpx4_isa_nejmpi(Vpx4* vpx4){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 != r2
    //===========================================
    //C syntax: if(registers[r1] != registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 != r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    
    if(val1 != val2){
        uint32_t val3 = vpx4_rreg(vpx4, r3);
        uint32_t valrpc = val3 + imm;
        vpx4_wreg(vpx4, VPX4_RPC, valrpc);
    }

}
void vpx4_isa_gjmpi(Vpx4* vpx4){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 > r2
    //===========================================
    //C syntax: if(registers[r1] > registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 > r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    
    if(val1 > val2){
        uint32_t val3 = vpx4_rreg(vpx4, r3);
        uint32_t valrpc = val3 + imm;
        vpx4_wreg(vpx4, VPX4_RPC, valrpc);
    }

}
void vpx4_isa_gejmpi(Vpx4* vpx4){
    //===========================================
    //Conditional Jump to address defined by: PC = (r3 + imm) IF: r1 >= r2
    //===========================================
    //C syntax: if(registers[r1] >= registers[r2]){registers[rpc] = registers[r3] + imm;}
    //Pseudocode: if r1 >= r2: rpc = r3 + imm
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint8_t r3 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    
    if(val1 >= val2){
        uint32_t val3 = vpx4_rreg(vpx4, r3);
        uint32_t valrpc = val3 + imm;
        vpx4_wreg(vpx4, VPX4_RPC, valrpc);
    }

}

void vpx4_isa_push8(Vpx4* vpx4){
    //===========================================
    //Push 8 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu8(registers[r1] & 0xff);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_rreg(vpx4, r1);

    vpx4_mem_pu08(vpx4, val1 & 0xff);
}
void vpx4_isa_push16(Vpx4* vpx4){
    //===========================================
    //Push 16 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu16(registers[r1] & 0xffff);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_rreg(vpx4, r1);

    vpx4_mem_pu16(vpx4, val1 & 0xffff);
}
void vpx4_isa_push32(Vpx4* vpx4){
    //===========================================
    //Push 32 bit value from r1 to stack.
    //===========================================
    //C syntax: mem_pu32(registers[r1]);
    //Pseudocode: stack[rsp] = r1; rsp++
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_rreg(vpx4, r1);

    vpx4_mem_pu32(vpx4, val1);
}
void vpx4_isa_pop8(Vpx4* vpx4){
    //===========================================
    //Pop 8 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po8();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_mem_po08(vpx4);

    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_pop16(Vpx4* vpx4){
    //===========================================
    //Pop 16 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po16();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_mem_po16(vpx4);

    vpx4_wreg(vpx4, r1, val1);
}
void vpx4_isa_pop32(Vpx4* vpx4){
    //===========================================
    //Pop 32 bit value to r1 from stack.
    //===========================================
    //C syntax: registers[r1] = mem_po32();
    //Pseudocode: rsp--; r1 = stack[rsp]
    //===========================================
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t val1 = vpx4_mem_po32(vpx4);

    vpx4_wreg(vpx4, r1, val1);
}

void vpx4_isa_calli(Vpx4* vpx4){
    //===========================================
    //Jump to address defined by: PC = (r1 + imm) And push current address to Stack (after fetching)
    //===========================================
    //C syntax: mem_pu32(registers[rpc]); registers[rpc] = registers[r1] + imm;
    //Pseudocode: stack[rsp] = rpc; rsp++; rpc = r1 + imm
    //===========================================
    //No need for adding pseudo code descriptions for these, Since it's a bit obvious.
    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint32_t imm = vpx4_mem_f32(vpx4);
    uint32_t ret_adr = vpx4_rreg(vpx4, VPX4_RPC) - 1; //Might be an off by 1 error. Change (-1) if so is the case.

    //PUSH
    vpx4_mem_pu32(vpx4, ret_adr);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t valrpc = val1 + imm;

    vpx4_wreg(vpx4, VPX4_RPC, valrpc);
}
void vpx4_isa_callr(Vpx4* vpx4){
    //===========================================
    //Jump to address defined by: PC = (r1 + r2) And push current address to Stack (after fetching)
    //===========================================

    uint8_t r1 = vpx4_mem_f08(vpx4);
    uint8_t r2 = vpx4_mem_f08(vpx4);
    uint32_t ret_adr = vpx4_rreg(vpx4, VPX4_RPC) - 1; //Might be an off by 1 error. Change (-1) if so is the case.

    //PUSH
    vpx4_mem_pu32(vpx4, ret_adr);

    uint32_t val1 = vpx4_rreg(vpx4, r1);
    uint32_t val2 = vpx4_rreg(vpx4, r2);
    uint32_t valrpc = val1 + val2;

    vpx4_wreg(vpx4, VPX4_RPC, valrpc);
}
void vpx4_isa_ret(Vpx4* vpx4){
    //===========================================
    //Jump to address in first 32 bits from stack (popped)
    //===========================================

    uint32_t ret_adr = vpx4_mem_po32(vpx4);


    vpx4_wreg(vpx4, VPX4_RPC, ret_adr);
}

