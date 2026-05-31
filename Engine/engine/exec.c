#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"


#include "context.h"



static inline uint32_t
load32(ctInstructionSize* instrs, uint64_t* ip) {
	uint32_t u;
	memcpy(&u, &instrs[*ip], sizeof(u));
	*ip += 4;
	return u;
}


static inline void 
out(ctAtom atom, ctAtomType type, uint32_t fmt) {

	if (fmt == 0) {
		
		switch (type) {

			case ctAtomType_NoneType:
				printf("[ None ]\n"); break;
			case ctAtomType_Int:
				printf("[ Int %ld ]\n", atom.as_int); break;
			case ctAtomType_UInt:
				printf("[ UInt %lu ]\n", atom.as_uint); break;
			case ctAtomType_Float:
				printf("[ Float %f ]\n",  atom.as_float); break;
			case ctAtomType_Bool:
				printf("[ Bool %s ]\n",  atom.as_bool ? "true" : "false"); break;
			case ctAtomType_Container:
				printf("[ Container ]\n"); break;
			break;
		}

	} else if (fmt == 1) {
		for (int i = 63; i >= 0; i--)
			putchar((atom.raw >> i) & 1 ? '1' : '0');
		putchar('\n');
	} else { 
		printf("Unknown format: %d\n", fmt);
	}

}


static inline const char*
type_name(ctAtomType type) {
	switch (type) {
		case ctAtomType_NoneType: return "NoneType";
		case ctAtomType_Int: return "Int";
		case ctAtomType_UInt: return "UInt";
		case ctAtomType_Float: return "Float";
		case ctAtomType_Bool: return "Bool";
		case ctAtomType_Container: return "Container";
		default: return "Unknown";
	}
}


static inline void
check_type(ctRegisterFile* registers, uint index, ctAtomType expected_type) {
	if (registers->types[index] != expected_type) {
		printf(
			"Expected type '%s'. Got '%s'\n",
			type_name(expected_type),
			type_name(registers->types[index])
		);
		exit(1);
	};
}


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


#define INSTR_CMP(Type, AtomField) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r1, Type); \
check_type(&ctx->registers, r2, Type); \
ctx->registers.atoms[r1].as_int =  ctx->registers.atoms[r1].AtomField - ctx->registers.atoms[r2].AtomField;


#define INSTR_CMPRESOLVERS(Operation) \
r1 = instrs[ctx->ip++]; \
if (ctx->registers.atoms[r1].as_int Operation 0) {ctx->registers.atoms[r1].as_bool = 1;} \
else {ctx->registers.atoms[r1].as_bool = 0;} \
ctx->registers.types[r1] = ctAtomType_Bool;


#define INSTR_JMP() \
r1 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r1, ctAtomType_Int); \
ctx->ip += ctx->registers.atoms[r1].as_int; \
if (ctx->ip >= ctx->image->header.instruction_count) {printf("Out of range ip\n"); exit(1);};


#define INSTR_JMPABS() \
r1 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r1, ctAtomType_Int); \
ctx->ip = ctx->registers.atoms[r1].as_int; \
if (ctx->ip >= ctx->image->header.instruction_count) {printf("Out of range ip\n"); exit(1);};


void
ct_ctx_exec(ctContext* ctx) 
{
	ctInstructionSize* instrs = ctx->image->instruction_pool;
	
	uint64_t r1;
	uint64_t r2;
	uint64_t r3;

	float f;
	int32_t i;
	uint32_t u;

	while(ctx->running)
	{
	ctInstruction instr = instrs[ctx->ip++];
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
			out(ctx->registers.atoms[r1], ctx->registers.types[r1], r2);
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
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&i, &u, sizeof(i)); 
			ctx->registers.atoms[r1].as_int = i;
			ctx->registers.types[r1] = ctAtomType_Int;
			break;

		case instrLoadU:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			ctx->registers.atoms[r1].as_uint = u;
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		case instrLoadF:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&f, &u, sizeof(f)); 
			ctx->registers.atoms[r1].as_float= f;
			ctx->registers.types[r1] = ctAtomType_Float;
			break;

		case instrLoadB:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			ctx->registers.atoms[r1].as_bool = u;
			ctx->registers.types[r1] = ctAtomType_Bool;
			break;

		case instrAddI:
			INSTR_BINARYOP(ctAtomType_Int, as_int, +);
			break;

		case instrSubI:
			INSTR_BINARYOP(ctAtomType_Int, as_int, -);
			break;

		case instrMulI:
			INSTR_BINARYOP(ctAtomType_Int, as_int, *);
			break;

		case instrDivI:
			INSTR_BINARYOP(ctAtomType_Int, as_int, /);
			break;

		case instrModI:
			INSTR_BINARYOP(ctAtomType_Int, as_int, %);
			break;

		case instrNegI:
			INSTR_UNARYOP(ctAtomType_Int, as_int, -);
			break;

		case instrAddU:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, +);
			break;

		case instrSubU:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, -);
			break;

		case instrMulU:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, *);
			break;

		case instrDivU:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, /);
			break;

		case instrModU:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, %);
			break;

		case instrAddF:
			INSTR_BINARYOP(ctAtomType_Float, as_float, +);
			break;	

		case instrSubF:
			INSTR_BINARYOP(ctAtomType_Float, as_float, -);
			break;	

		case instrMulF:
			INSTR_BINARYOP(ctAtomType_Float, as_float, *);
			break;	

		case instrDivF:
			INSTR_BINARYOP(ctAtomType_Float, as_float, /);
			break;	

		case instrNegF:
			INSTR_UNARYOP(ctAtomType_Float, as_float, -);
			break;

		case instrLogicAnd:
			INSTR_BINARYOP(ctAtomType_Bool, as_bool, &&);
			break;

		case instrLogicOr:
			INSTR_BINARYOP(ctAtomType_Bool, as_bool, ||);
			break;

		case instrLogicNot:
			INSTR_UNARYOP(ctAtomType_Bool, as_bool, !);
			break;

		case instrBitAnd:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, &);
			break;

		case instrBitOr:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, |);
			break;

		case instrBitNot:
			INSTR_UNARYOP(ctAtomType_UInt, as_uint, ~);
			break;

		case instrBitXor:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, ^);
			break;

		case instrBitShl:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, <<);
			break;
			
		case instrBitShr:
			INSTR_BINARYOP(ctAtomType_UInt, as_uint, >>);
			break;

		case instrCmpI:
			INSTR_CMP(ctAtomType_Int, as_int);
			break;

		case instrCmpU:
			INSTR_CMP(ctAtomType_UInt, as_float);
			break;

		case instrCmpF:
			INSTR_CMP(ctAtomType_Float, as_uint);
			break;

		case instrEq:
			INSTR_CMPRESOLVERS(==);
			break;

		case instrNotEq:
			INSTR_CMPRESOLVERS(!=);
			break;

		case instrLess:
			INSTR_CMPRESOLVERS(<);
			break;

		case instrLessEq:
			INSTR_CMPRESOLVERS(<=);
			break;

		case instrGreater:
			INSTR_CMPRESOLVERS(>);
			break;

		case instrGreaterEq:
			INSTR_CMPRESOLVERS(>=);
			break;

		case instrJmp:
			INSTR_JMP();
			break;

		case instrJmpIf:
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Bool);
			if (ctx->registers.atoms[r2].as_bool) {
				INSTR_JMP();
				continue;
			}
			ctx->ip++;
			break;
			
		case instrJmpIfNot:
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Bool);
			if (!ctx->registers.atoms[r2].as_bool) {
				INSTR_JMP();
				continue;
			}
			ctx->ip++;
			break;

		case instrJmpAbs:
			INSTR_JMPABS();
			break;

		case instrJmpAbsIf:
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Bool);
			if (ctx->registers.atoms[r2].as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip++;
			break;
			
		case instrJmpAbsIfNot:
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Bool);
			if (!ctx->registers.atoms[r2].as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip++;
			break;

		case instrCall:
			r1 = load32(instrs, &ctx->ip);
			ct_ctx_callProcedure(ctx, r1);
			break;

		case instrReturn:
			ct_ctx_returnProcedure(ctx);
			break;

		default:
			ct_ctx_reportFailure(ctx, (ctFailure){"Illegal Instruction."});
		}
	}

}
