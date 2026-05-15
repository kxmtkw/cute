#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuteByte.h"
#include "CuteAtom.h"
#include "CuteEngine.h"

static inline uint32_t
load32(CtInstructionSize* instrs, uint64_t* ip) {
	uint32_t u;
	memcpy(&u, &instrs[*ip], sizeof(u));
	*ip += 4;
	return u;
}

void 
out(uint64_t value, uint32_t fmt) {
    switch (fmt) {
        case 0:
            for (int i = 63; i >= 0; i--)
                putchar((value >> i) & 1 ? '1' : '0');
            putchar('\n');
            break;
        case 1: printf("%lx\n",  value); break;
        case 2: printf("%ld\n",  value); break;
        case 3: printf("%lu\n",  (uint64_t)value); break;
        case 4: double f; memcpy(&f, &value, 8); printf("%f\n", f); break;
        case 5: printf("%s\n",   value ? "true" : "false"); break;
		default: printf("Unknown format: %d\n", fmt); break;
    }
}


void
Cute_exec(CtContext* ctx) 
{

CtInstructionSize* instrs = ctx->image->instruction_pool;
	
uint64_t r1;
uint64_t r2;
uint64_t r3;

double f1;
double f2;

while(true)
{
CtInstruction instr = instrs[ctx->ip++];
//printf("instruction 0x%x ip: %lu\n", instr, ctx->ip);

switch (instr) 
{

case instrHalt:
	exit(instrs[ctx->ip++]);
	return;

case instrNull:
	continue;

case instrOut:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	out(ctx->registers[r1], r2);
	break;

case instrMov:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r2] = ctx->registers[r1];
	break;

case instrLoadI:
	r1 = load32(instrs, &ctx->ip);
	r1 = CTATOM_MASK(r1, CtAtomType_Int);
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r2);
	ctx->registers[r2] = r1;
	break;

case instrLoadU:
	r1 = load32(instrs, &ctx->ip);
	r1 = CTATOM_MASK(r1, CtAtomType_UInt);
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r2);
	ctx->registers[r2] = r1;
	break;

case instrLoadF:
	r1 = load32(instrs, &ctx->ip);
	r1 = CTATOM_MASK(r1, CtAtomType_Float);
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r2);
	ctx->registers[r2] = r1;
	break;

case instrAddI:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (int64_t)ctx->registers[r1] + (int64_t)ctx->registers[r2];
	break;

case instrSubI:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (int64_t)ctx->registers[r1] - (int64_t)ctx->registers[r2];
	break;

case instrMulI:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (int64_t)ctx->registers[r1] * (int64_t)ctx->registers[r2];
	break;

case instrDivI:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (int64_t)ctx->registers[r1] / (int64_t)ctx->registers[r2];
	break;

case instrModI:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (int64_t)ctx->registers[r1] % (int64_t)ctx->registers[r2];
	break;

case instrNegI:
	r1 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	ctx->registers[r1] = -(int64_t)ctx->registers[r1];
	break;

case instrDivU:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (uint64_t)ctx->registers[r1] / (uint64_t)ctx->registers[r2];
	break;

case instrModU:
	r1 = instrs[ctx->ip++];
	r2 = instrs[ctx->ip++];
	CT_VALIDREGISTER(r1);
	CT_VALIDREGISTER(r2);
	ctx->registers[r1] = (uint64_t)ctx->registers[r1] % (uint64_t)ctx->registers[r2];
	break;

case instrAddF:
    r1 = instrs[ctx->ip++];
    r2 = instrs[ctx->ip++];
    CT_VALIDREGISTER(r1);
    CT_VALIDREGISTER(r2);
    memcpy(&f1, &ctx->registers[r1], 8);
    memcpy(&f2, &ctx->registers[r2], 8);
    f1 += f2;
    memcpy(&ctx->registers[r1], &f1, 8);
	printf("%f\n", f1);
    break;

case instrSubF:
    r1 = instrs[ctx->ip++];
    r2 = instrs[ctx->ip++];
    CT_VALIDREGISTER(r1);
    CT_VALIDREGISTER(r2);
    memcpy(&f1, &ctx->registers[r1], 8);
    memcpy(&f2, &ctx->registers[r2], 8);
    f1 -= f2;
    memcpy(&ctx->registers[r1], &f1, 8);
    break;

case instrMulF:
    r1 = instrs[ctx->ip++];
    r2 = instrs[ctx->ip++];
    CT_VALIDREGISTER(r1);
    CT_VALIDREGISTER(r2);
    memcpy(&f1, &ctx->registers[r1], 8);
    memcpy(&f2, &ctx->registers[r2], 8);
    f1 *= f2;
    memcpy(&ctx->registers[r1], &f1, 8);
    break;

case instrDivF:
    r1 = instrs[ctx->ip++];
    r2 = instrs[ctx->ip++];
    CT_VALIDREGISTER(r1);
    CT_VALIDREGISTER(r2);
    memcpy(&f1, &ctx->registers[r1], 8);
    memcpy(&f2, &ctx->registers[r2], 8);
    f1 /= f2;
    memcpy(&ctx->registers[r1], &f1, 8);
    break;

case instrNegF:
    r1 = instrs[ctx->ip++];
    CT_VALIDREGISTER(r1);
    memcpy(&f1, &ctx->registers[r1], 8);
    f1 = -f1;
    memcpy(&ctx->registers[r1], &f1, 8);
    break;

case instrCmpI:
case instrCmpF:
case instrCheckEq:
case instrCheckLt:
case instrCheckLe:
case instrCheckGt:
case instrCheckGe:
case instrJmp:
	r1 = instrs[ctx->ip];
	ctx->ip = r1;
	break;
case instrJmpIf:
case instrJmpIfNot:
case instrCall:
case instrRet:
case instrLogAnd:
case instrLogOr:
case instrLogNot:
case instrBitAnd:
case instrBitOr:
case instrBitXor:
case instrBitNot:
case instrBitShL:
case instrBitShR:
case instrBitSaL:
case instrBitSaR:
case instrConNew:
case instrConDel:
case instrConGet:
case instrConSet:
case instrConClone:
	break;

}

}

}