#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"


#include "containers/container.h"
#include "context.h"
#include "engine/error.h"


static inline uint32_t
load32(ctInstructionSize* instrs, uint64_t* ip) {
	uint32_t u;
	memcpy(&u, &instrs[*ip], sizeof(u));
	*ip += 4;
	return u;
}

static inline void 
out(ctAtom atom, ctAtomTypeSize type) {

	const char* name = ct_atom_stringforms[type];

	switch (type) {
		
		case ctAtomType_NoneType:
			printf("[ %s ]\n", name); break;
		case ctAtomType_Int:
			printf("[ %s %ld ]\n", name, atom.as_int); break;
		case ctAtomType_UInt:
			printf("[ %s %lu ]\n", name, atom.as_uint); break;
		case ctAtomType_Float:
			printf("[ %s %f ]\n", name, atom.as_float); break;
		case ctAtomType_Bool:
			printf("[ %s %u ]\n", name, atom.as_bool ? 1 : 0); break;
		case ctAtomType_Char:
			printf("[ %s %c ]\n", name, atom.as_char); break;
		case ctAtomType_Container:
			printf("[ %s %p ]\n", name, atom.as_container); break;
		break;
	}
}


static inline void
check_type(ctRegisterFile* registers, uint index, ctAtomType expected_type) {
	if (registers->types[index] != expected_type) {
		printf(
			"Expected type '%s'. Got '%s'\n",
			ct_atom_stringforms[expected_type],
			ct_atom_stringforms[registers->types[index]]
		);
		exit(1);
	};
}

// todo: checking destination type to ensure it isn't a container

#define INSTR_BINARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r2, Type); \
check_type(&ctx->registers, r3, Type); \
ctx->registers.atoms[r1].AtomField =  ctx->registers.atoms[r2].AtomField Operation ctx->registers.atoms[r3].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_BINARYOP_DIV(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r2, Type); \
check_type(&ctx->registers, r3, Type); \
if (ctx->registers.atoms[r3].AtomField == 0) { \
	ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_ZeroDivision, "Tried to divide by zero.")); \
	return; \
} \
ctx->registers.atoms[r1].AtomField =  ctx->registers.atoms[r2].AtomField Operation ctx->registers.atoms[r3].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_UNARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r2, Type); \
ctx->registers.atoms[r1].AtomField = Operation ctx->registers.atoms[r2].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_CMP(Type, AtomField) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
check_type(&ctx->registers, r2, Type); \
check_type(&ctx->registers, r3, Type); \
ctx->registers.atoms[r1].as_int =  ctx->registers.atoms[r2].AtomField - ctx->registers.atoms[r3].AtomField; \
ctx->registers.types[r1] = ctAtomType_Int;


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

#define CONTAINER_CHECKS(Index, Function) \
if (ctx->registers.types[Index] == ctAtomType_Container) { \
	Function(ctx->containers, ctx->registers.atoms[Index].as_container); \
} 

void
ct_ctx_exec(ctContext* ctx) 
{
	// local variables needed for execuction

	ctInstructionSize* instrs = ctx->image->instruction_pool;
	ctRegisterFile* registers = &ctx->registers;

	uint8_t r1;
	uint8_t r2;
	uint8_t r3;

	float f;
	int32_t i;
	uint32_t u;
	ctContainer* con;

	ctTypedAtom typed;

	while(ctx->running)
	{
	ctInstruction instr = instrs[ctx->ip++];
	CUTE_LOG("trace", "ip: %08lu | instr: 0x%02X | ctx: %p\n", ctx->ip-1, instr, ctx);

		switch (instr) 
		{

		case instrHalt:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_UInt);
			ctx->exit_code = ctx->registers.atoms[r1].as_uint;
			return;

		case instrNull:
			continue;

		case instrOut:
			r1 = instrs[ctx->ip++];
			out(ctx->registers.atoms[r1], ctx->registers.types[r1]);
			break;

		case instrMov:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ctx->registers.atoms[r1] = ctx->registers.atoms[r2];
			ctx->registers.types[r1] = ctx->registers.types[r2];
			break;

		// add proper load/store
		case instrLoad:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ctx->registers.atoms[r1] = ctx->registers.atoms[r2];
			ctx->registers.types[r1] = ctx->registers.types[r2];
			break;

		case instrStore:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ctx->registers.atoms[r1] = ctx->registers.atoms[r2];
			ctx->registers.types[r1] = ctx->registers.types[r2];
			break;

		case instrSetI:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&i, &u, sizeof(i));
			ctx->registers.atoms[r1].as_int = i;
			ctx->registers.types[r1] = ctAtomType_Int;
			break;

		case instrSetU:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			ctx->registers.atoms[r1].as_uint = u;
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		case instrSetF:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&f, &u, sizeof(f));
			ctx->registers.atoms[r1].as_float = f;
			ctx->registers.types[r1] = ctAtomType_Float;
			break;

		case instrSetB:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			ctx->registers.atoms[r1].as_bool = u;
			ctx->registers.types[r1] = ctAtomType_Bool;
			break;

		case instrSetC:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			ctx->registers.atoms[r1].as_char = u;
			ctx->registers.types[r1] = ctAtomType_Char;
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
			INSTR_BINARYOP_DIV(ctAtomType_Int, as_int, /);
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
			INSTR_BINARYOP_DIV(ctAtomType_UInt, as_uint, /);
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
			INSTR_BINARYOP_DIV(ctAtomType_Float, as_float, /);
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
			INSTR_CMP(ctAtomType_UInt, as_uint);
			break;

		case instrCmpF:
			INSTR_CMP(ctAtomType_Float, as_float);
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

		case instrConNew:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_UInt);
			u = ctx->registers.atoms[r2].as_uint;
			con = ct_containers_newContainer(ctx->containers, u);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;
			
		case instrConDel:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_Container);
			con = ctx->registers.atoms[r1].as_container;
			ct_containers_decRef(ctx->containers, con);
			ctx->registers.types[r1] = ctAtomType_NoneType;
			break;

		case instrConGet:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Container);
			r3 = instrs[ctx->ip++];
			check_type(&ctx->registers, r3, ctAtomType_UInt);
			con = ctx->registers.atoms[r2].as_container;

			typed = ct_containers_conGet(ctx->containers, con, ctx->registers.atoms[r3].as_uint, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}

			ctx->registers.atoms[r1] = typed.atom;
			ctx->registers.types[r1] = typed.type;
			break;

		case instrConSet:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_Container);
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_UInt);
			r3 = instrs[ctx->ip++];
			typed = (ctTypedAtom){ctx->registers.types[r3], ctx->registers.atoms[r3]};
			
			con = ctx->registers.atoms[r1].as_container;
			ct_containers_conSet(ctx->containers, con, ctx->registers.atoms[r2].as_uint, typed, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}

			break;

		case instrConClone:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Container);
			con = ct_containers_conClone(
				ctx->containers, ctx->registers.atoms[r2].as_container, &ctx->error
			);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;

		case instrConResize:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_Container);
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_UInt);
			ct_containers_conResize(ctx->containers, ctx->registers.atoms[r1].as_container, ctx->registers.atoms[r2].as_uint, &ctx->error);
			break;

		case instrConLen:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_Container);
			con = ctx->registers.atoms[r2].as_container;
			ctx->registers.atoms[r1].as_uint = con->size;
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		default:
			ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "illegal instruction executed. halting engine."));
		}
	}

}
