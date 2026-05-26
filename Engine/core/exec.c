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


static inline void 
out(CtAtom atom, uint32_t fmt) {
    switch (fmt) {
        case 0:
            for (int i = 63; i >= 0; i--)
                putchar((atom.raw >> i) & 1 ? '1' : '0');
            putchar('\n');
            break;
        case 1: printf("%lx\n",  atom.as_uint); break;
        case 2: printf("%ld\n", atom.as_int); break;
        case 3: printf("%lu\n", atom.as_uint); break;
		case 4: printf("%f\n",  atom.as_float); break;
        case 5: printf("%s\n",   atom.raw ? "true" : "false"); break;
		default: printf("Unknown format: %d\n", fmt); break;
    }
}


static inline const char*
type_name(CtAtomType type) {
	switch (type) {
		case CtAtomType_NoneType: return "NoneType";
		case CtAtomType_Int: return "Int";
		case CtAtomType_UInt: return "UInt";
		case CtAtomType_Float: return "Float";
		case CtAtomType_Bool: return "Bool";
		case CtAtomType_Container: return "Container";
		default: return "Unknown";
	}
}


static inline void
check_type(CtRegisterFile* registers, uint index, CtAtomType expected_type) {
	if (registers->types[index] != expected_type) {
		printf(
			"Expected type '%s'. Got '%s'\n",
			type_name(expected_type),
			type_name(registers->types[index])
		);
		exit(1);
	};
}


#define INSTR_LOAD(type) \
r1 = instrs[ctx->ip++]; \
r2 = load32(instrs, &ctx->ip); \
ctx->registers.atoms[r1].raw = r2; \
ctx->registers.types[r1] = type;


#define INSTR_BINARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r1, Type); \
check_type(&ctx->registers, r2, Type); \
ctx->registers.atoms[r1].AtomField =  ctx->registers.atoms[r1].AtomField Operation ctx->registers.atoms[r2].AtomField;


#define INSTR_UNARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r1, Type); \
ctx->registers.atoms[r1].AtomField = Operation ctx->registers.atoms[r1].AtomField;


void
Cute_exec(CtContext* ctx) 
{
	CtInstructionSize* instrs = ctx->image->instruction_pool;
		
	uint64_t r1;
	uint64_t r2;
	uint64_t r3;

	float f;

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
			out(ctx->registers.atoms[r1], r2);
			break;

		case instrTypeOf:
			r1 = instrs[ctx->ip++];
			printf("%s\n", type_name(ctx->registers.types[r1]));
			break;

		case instrMov:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ctx->registers.atoms[r1] = ctx->registers.atoms[r2];
			ctx->registers.types[r1] = ctx->registers.types[r2];
			break;

		case instrLoadI:
			INSTR_LOAD(CtAtomType_Int);
			break;

		case instrLoadU:
			INSTR_LOAD(CtAtomType_UInt);
			break;

		case instrLoadF:
			// This is temporary. Once load instruction actually loads from a constant pool,
			// this won't be needed.
			r1 = instrs[ctx->ip++];
			r2 = load32(instrs, &ctx->ip);
			memcpy(&f, &r2, sizeof(f)); 
			ctx->registers.atoms[r1].as_float= f;
			ctx->registers.types[r1] = CtAtomType_Float;
			break;

		case instrLoadB:
			INSTR_LOAD(CtAtomType_Bool);
			break;

		case instrAddI:
			INSTR_BINARYOP(CtAtomType_Int, as_int, +);
			break;

		case instrSubI:
			INSTR_BINARYOP(CtAtomType_Int, as_int, -);
			break;

		case instrMulI:
			INSTR_BINARYOP(CtAtomType_Int, as_int, *);
			break;

		case instrDivI:
			INSTR_BINARYOP(CtAtomType_Int, as_int, /);
			break;

		case instrModI:
			INSTR_BINARYOP(CtAtomType_Int, as_int, %);
			break;

		case instrNegI:
			INSTR_UNARYOP(CtAtomType_Int, as_int, -);
			break;

		case instrAddU:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, +);
			break;

		case instrSubU:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, -);
			break;

		case instrMulU:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, *);
			break;

		case instrDivU:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, /);
			break;

		case instrModU:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, %);
			break;

		case instrAddF:
			INSTR_BINARYOP(CtAtomType_Float, as_float, +);
			break;	

		case instrSubF:
			INSTR_BINARYOP(CtAtomType_Float, as_float, -);
			break;	

		case instrMulF:
			INSTR_BINARYOP(CtAtomType_Float, as_float, *);
			break;	

		case instrDivF:
			INSTR_BINARYOP(CtAtomType_Float, as_float, /);
			break;	

		case instrNegF:
			INSTR_UNARYOP(CtAtomType_Float, as_float, -);
			break;

		case instrLogicAnd:
			INSTR_BINARYOP(CtAtomType_Bool, as_bool, &&);
			break;

		case instrLogicOr:
			INSTR_BINARYOP(CtAtomType_Bool, as_bool, ||);
			break;

		case instrLogicNot:
			INSTR_UNARYOP(CtAtomType_Bool, as_bool, !);
			break;

		case instrBitAnd:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, &);
			break;

		case instrBitOr:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, |);
			break;

		case instrBitNot:
			INSTR_UNARYOP(CtAtomType_UInt, as_uint, ~);
			break;

		case instrBitXor:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, ^);
			break;

		case instrBitShl:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, <<);
			break;
			
		case instrBitShr:
			INSTR_BINARYOP(CtAtomType_UInt, as_uint, >>);
			break;
		}
	}

}
