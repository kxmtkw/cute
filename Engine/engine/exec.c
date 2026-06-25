#include <math.h>
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


static inline void
loadBytes(ctInstructionSize* instrs, uint64_t* ip, uint32_t n, void* dest) {
	memcpy(dest, &instrs[*ip], n);
	*ip += n;
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



#define CHECK_TYPE(INDEX, TYPE) \
if (ctx->registers.types[INDEX] != TYPE) { \
	ct_ctx_throwError( \
		ctx, \
		ct_error_make(ctErrorCode_TypeError, "Unexpected Type.") \
	); \
	printf("Got %s, expected %s\n", ct_atom_stringforms[ctx->registers.types[INDEX]], ct_atom_stringforms[TYPE]);\
	return; \
};

#define DEC_IF_CONTAINER(Index) \
if (registers->types[Index] == ctAtomType_Container) { \
	ct_containers_decRef(ctx->containers, ctx->registers.atoms[Index].as_container); \
} 

#define INC_IF_CONTAINER(Index) \
if (ctx->registers.types[Index] == ctAtomType_Container) { \
	ct_containers_incRef(ctx->containers, ctx->registers.atoms[Index].as_container); \
} 


#define INSTR_BINARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
CHECK_TYPE(r2, Type); \
CHECK_TYPE(r3, Type); \
DEC_IF_CONTAINER(r1); \
ctx->registers.atoms[r1].AtomField =  ctx->registers.atoms[r2].AtomField Operation ctx->registers.atoms[r3].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_BINARYOP_DIV(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
CHECK_TYPE(r2, Type); \
CHECK_TYPE(r3, Type); \
if (ctx->registers.atoms[r3].AtomField == 0) { \
	ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_ZeroDivision, "Tried to divide by zero.")); \
	return; \
} \
DEC_IF_CONTAINER(r1); \
ctx->registers.atoms[r1].AtomField =  ctx->registers.atoms[r2].AtomField Operation ctx->registers.atoms[r3].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_UNARYOP(Type, AtomField, Operation) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
CHECK_TYPE(r2, Type); \
DEC_IF_CONTAINER(r1); \
ctx->registers.atoms[r1].AtomField = Operation ctx->registers.atoms[r2].AtomField; \
ctx->registers.types[r1] = Type;


#define INSTR_CMP(Type, AtomField) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
CHECK_TYPE(r1, Type); \
CHECK_TYPE(r2, Type); \
ctx->cmp_diff =  ctx->registers.atoms[r1].AtomField - ctx->registers.atoms[r2].AtomField;


#define INSTR_CMPRESOLVERS(Operation) \
r1 = instrs[ctx->ip++]; \
DEC_IF_CONTAINER(r1); \
if (ctx->cmp_diff Operation 0) {ctx->registers.atoms[r1].as_bool = 1;} \
else {ctx->registers.atoms[r1].as_bool = 0;} \
ctx->registers.types[r1] = ctAtomType_Bool;


#define INSTR_JMP() \
loadBytes(instrs, &ctx->ip, 4, &i); \
ctx->ip += i; \
if (ctx->ip >= ctx->image->header.instruction_count) {ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "Out of range ip.")); return;};

#define INSTR_JMPABS() \
loadBytes(instrs, &ctx->ip, 4, &u); \
ctx->ip = u; \
if (ctx->ip >= ctx->image->header.instruction_count) {ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "Out of range ip.")); return;};



void
ct_ctx_exec(ctContext* ctx) 
{
	ctInstructionSize* instrs = ctx->image->instruction_pool;
	ctRegisterFile* registers = &ctx->registers;
	
	uint8_t r1, r2, r3, r4;

	int32_t  i;
	uint32_t u;
	float   f;
	ctContainer* con;
	ctTypedAtom typed_atom;

	ctInstruction instr;

	while (ctx->running) {
		instr = instrs[ctx->ip++];
		CUTE_LOG("trace", "ip: %08lu | instr: 0x%02X | ctx: %p\n", ctx->ip-1, instr, ctx);

		switch (instr) 
		{

		case instrHalt:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_UInt);
			ctx->exit_code = ctx->registers.atoms[r1].as_uint;
			return;

		case instrNull:
			continue;

		case instrOut:
			r1 = instrs[ctx->ip++];
			out(ctx->registers.atoms[r1], ctx->registers.types[r1]);
			break;

		case instrTypeof:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_uint = ctx->registers.types[r2];
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		case instrMov:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1] = ctx->registers.atoms[r2];
			ctx->registers.types[r1] = ctx->registers.types[r2];
			INC_IF_CONTAINER(r1);
			break;

		case instrLoad:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			typed_atom = ct_ctx_loadAtom(ctx, r2);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1] = typed_atom.atom;
			ctx->registers.types[r1] = typed_atom.type;
			INC_IF_CONTAINER(r1);
			break;

		case instrStore:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			typed_atom.atom = ctx->registers.atoms[r2];
			typed_atom.type = ctx->registers.types[r2];
			ct_ctx_storeAtom(ctx, r1, typed_atom);
			break;

		case instrSetI:
			r1 = instrs[ctx->ip++];
			loadBytes(instrs, &ctx->ip, 4, &i);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_int = i;
			ctx->registers.types[r1] = ctAtomType_Int;
			break;

		case instrSetU:
			r1 = instrs[ctx->ip++];
			loadBytes(instrs, &ctx->ip, 4, &u);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_uint = u;
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		case instrSetF:
			r1 = instrs[ctx->ip++];
			loadBytes(instrs, &ctx->ip, 4, &f);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_float = f;
			ctx->registers.types[r1] = ctAtomType_Float;
			break;

		case instrSetB:
			r1 = instrs[ctx->ip++];
			loadBytes(instrs, &ctx->ip, 4, &u);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_bool = u;
			ctx->registers.types[r1] = ctAtomType_Bool;
			break;

		case instrSetC:
			r1 = instrs[ctx->ip++];
			loadBytes(instrs, &ctx->ip, 4, &u);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_char = u;
			ctx->registers.types[r1] = ctAtomType_Char;
			break;

		case instrSetN:
			r1 = instrs[ctx->ip++];
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_uint = 0;
			ctx->registers.types[r1] = ctAtomType_NoneType;
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

		case instrAbsI:
			r1 = instrs[ctx->ip++]; 
			r2 = instrs[ctx->ip++]; 
			CHECK_TYPE(r2, ctAtomType_Int); 
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_int = labs(ctx->registers.atoms[r2].as_int);
			ctx->registers.types[r1] = ctAtomType_Int;
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

		case instrAbsF:
			r1 = instrs[ctx->ip++]; 
			r2 = instrs[ctx->ip++]; 
			CHECK_TYPE(r2, ctAtomType_Float); 
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_float = fabs(ctx->registers.atoms[r2].as_float);
			ctx->registers.types[r1] = ctAtomType_Float;
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
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Bool);
			if (ctx->registers.atoms[r1].as_bool) {
				INSTR_JMP();
				continue;
			}
			ctx->ip += 4;
			break;
			
		case instrJmpIfNot:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Bool);
			if (!ctx->registers.atoms[r1].as_bool) {
				INSTR_JMP();
				continue;
			}
			ctx->ip += 4;
			break;

		case instrJmpAbs:
			INSTR_JMPABS();
			break;

		case instrJmpAbsIf:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_Bool);
			if (ctx->registers.atoms[r1].as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip += 4;
			break;
			
		case instrJmpAbsIfNot:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Bool);
			if (!ctx->registers.atoms[r1].as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip += 4;
			break;

		case instrCall:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_UInt);
			ct_ctx_callProcedure(ctx, ctx->registers.atoms[r1].as_uint);
			break;

		case instrReturn:
			ct_ctx_returnProcedure(ctx);
			break;

		case instrConNew:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_UInt);
			u = ctx->registers.atoms[r2].as_uint;
			con = ct_containers_newContainer(ctx->containers, u);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;
			
		case instrConDel:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Container);
			con = ctx->registers.atoms[r1].as_container;
			ct_containers_decRef(ctx->containers, con);
			ctx->registers.types[r1] = ctAtomType_NoneType;
			break;

		case instrConGet:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_Container);
			r3 = instrs[ctx->ip++];
			CHECK_TYPE(r3, ctAtomType_UInt);
			con = ctx->registers.atoms[r2].as_container;

			typed_atom = ct_containers_conGet(ctx->containers, con, ctx->registers.atoms[r3].as_uint, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}

			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1] = typed_atom.atom;
			ctx->registers.types[r1] = typed_atom.type;
			INC_IF_CONTAINER(r1);
			break;

		case instrConSet:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Container);
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_UInt);
			r3 = instrs[ctx->ip++];
			typed_atom = (ctTypedAtom){ctx->registers.types[r3], ctx->registers.atoms[r3]};
			
			con = ctx->registers.atoms[r1].as_container;
			ct_containers_conSet(ctx->containers, con, ctx->registers.atoms[r2].as_uint, typed_atom, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}

			break;

		case instrConLen:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_Container);
			con = ctx->registers.atoms[r2].as_container;
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_uint = con->size;
			ctx->registers.types[r1] = ctAtomType_UInt;
			break;

		case instrConResize:
			r1 = instrs[ctx->ip++];
			CHECK_TYPE(r1, ctAtomType_Container);
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_UInt);
			ct_containers_conResize(ctx->containers, ctx->registers.atoms[r1].as_container, ctx->registers.atoms[r2].as_uint, &ctx->error);
			break;

		case instrConCopy:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_Container);
			con = ct_containers_conCopy(
				ctx->containers, ctx->registers.atoms[r2].as_container, &ctx->error
			);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;

		case instrConClone:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			CHECK_TYPE(r2, ctAtomType_Container);
			con = ct_containers_conClone(
				ctx->containers, ctx->registers.atoms[r2].as_container, &ctx->error
			);
			DEC_IF_CONTAINER(r1);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;

		default:
			ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "illegal instruction executed. halting engine."));
		}
	}
}


#undef CHECK_TYPE
#undef INC_IF_CONTAINER
#undef DEC_IF_CONTAINER
#undef INSTR_BINARYOP
#undef INSTR_BINARYOP_DIV
#undef INSTR_UNARYOP
#undef INSTR_CMP
#undef INSTR_CMPRESOLVERS
#undef INSTR_JMP
#undef INSTR_JMPABS

