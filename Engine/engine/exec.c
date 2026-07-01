#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "CuteAtom.h"
#include "CuteConfig.h"
#include "CuteInstr.h"


#include "context.h"
#include "error.h"
#include "containers/container.h"



#define INSTR_BINARYOP(TYPE, FIELD, OP) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
r3 = instrs[ctx->ip++]; \
ct_ctx_loadAtom(ctx, r2, &a1, &t1); \
ct_ctx_loadAtom(ctx, r3, &a2, &t2); \
ct_ctx_storeAtom(ctx, r1, (ctAtom){.FIELD = a1.FIELD OP a2.FIELD}, TYPE);


#define INSTR_UNARYOP(TYPE, FIELD, OP) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
ct_ctx_loadAtom(ctx, r2, &a1, &t1); \
ct_ctx_storeAtom(ctx, r1, (ctAtom){.FIELD = OP (a1.FIELD)}, TYPE);


#define INSTR_CMP(TYPE, FIELD) \
r1 = instrs[ctx->ip++]; \
r2 = instrs[ctx->ip++]; \
ct_ctx_loadAtom(ctx, r1, &a1, &t1); \
ct_ctx_loadAtom(ctx, r2, &a2, &t2); \
ctx->cmp_diff = a1.FIELD - a2.FIELD; 


#define INSTR_CMP_RESOLVER(OP) \
r1 = instrs[ctx->ip++]; \
ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_bool = ctx->cmp_diff OP 0 ? 1 : 0}, ctAtomType_Bool); \


#define INSTR_JMP() \
ct_loadBytes(instrs, &ctx->ip, 4, &i32); \
ctx->ip += i32; \
if (ctx->ip >= ctx->image->header.instruction_count) {ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "Out of range ip.")); return;};


#define INSTR_JMPABS() \
ct_loadBytes(instrs, &ctx->ip, 4, &u32); \
ctx->ip = u32; \
if (ctx->ip >= ctx->image->header.instruction_count) {ct_ctx_throwError(ctx, ct_error_make(ctErrorCode_IllegalInstruction, "Out of range ip.")); return;};


static inline void
ct_loadBytes(ctInstructionSize* instrs, uint64_t* ip, uint32_t n, void* dest) {
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
		case ctAtomType_Container:
			printf("[ %s %p ]\n", name, atom.as_container); break;
		default:
			printf("[ unknown ]\n");
	}
}


void
ct_exec(ctContext* ctx) {

	ctInstructionSize* instrs = ctx->image->instruction_pool;

	uint8_t r1;
	uint8_t r2;
	uint8_t r3;
	uint8_t r4;

	int32_t  i32;
	uint32_t u32;
	float    f32;

	ctAtom a1;
	ctAtom a2;
	ctAtom a3;
	ctAtomType t1;
	ctAtomType t2;
	ctAtomType t3;

	ctTypedAtom typed_atom;

	while (ctx->running) {
		ctInstruction instr = instrs[ctx->ip++];
		CUTE_LOG("trace", "ip: %08lu | instr: 0x%02X | ctx: %p\n", ctx->ip-1, instr, ctx);

	switch (instr) {

		case instrNull:
			continue;

        case instrHalt:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ctx->exit_code = a1.as_uint;
			return;

        case instrAssert:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			if (!a1.as_bool) {
				ct_ctx_throwError(
					ctx,
					ct_error_make(ctErrorCode_AssertionFailed, "Assertion Failed.")
				);
			};
			return;

        case instrOut:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			out(a1, t1);
			break;

        case instrOutBits:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			for (int i = 63; i >= 0; i--) {
				printf("%d", (int)((a1.raw >> i) & 1));
				if (i % 8 == 0 && i != 0) printf(" ");
			}
			printf(" [ 0x%016lX ]\n", (uint64_t)a1.raw);
			break;

        case instrMov:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_moveAtom(ctx, r2, r1);
			break;

        case instrSetI:
			r1 = instrs[ctx->ip++];
			ct_loadBytes(instrs, &ctx->ip, 4, &i32);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_int=i32}, ctAtomType_Int);
			break;

        case instrSetU:
			r1 = instrs[ctx->ip++];
			ct_loadBytes(instrs, &ctx->ip, 4, &u32);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_uint=u32}, ctAtomType_UInt);
			break;

        case instrSetF:
			r1 = instrs[ctx->ip++];
			ct_loadBytes(instrs, &ctx->ip, 4, &f32);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_float=i32}, ctAtomType_Float);
			break;

        case instrSetB:
			r1 = instrs[ctx->ip++];
			ct_loadBytes(instrs, &ctx->ip, 4, &u32);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_bool=u32}, ctAtomType_Bool);
			break;

        case instrSetN:
			r1 = instrs[ctx->ip++];
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_uint=0}, ctAtomType_NoneType);
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

        case instrAbsI:
            INSTR_UNARYOP(ctAtomType_Int, as_int, labs);
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

        case instrAbsF:
            INSTR_UNARYOP(ctAtomType_Float, as_float, fabs);
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

        case instrLogicXor:
            INSTR_BINARYOP(ctAtomType_Bool, as_bool, ^);
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
			INSTR_CMP_RESOLVER(==);
			break;

		case instrNotEq:
			INSTR_CMP_RESOLVER(!=);
			break;

		case instrLess:
			INSTR_CMP_RESOLVER(<);
			break;

		case instrLessEq:
			INSTR_CMP_RESOLVER(<=);
			break;

		case instrGreater:
			INSTR_CMP_RESOLVER(>);
			break;

		case instrGreaterEq:
			INSTR_CMP_RESOLVER(>=);
			break;

		case instrJmp:
			INSTR_JMP();
			break;

		case instrJmpIf:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			if (a1.as_bool) {
				INSTR_JMP();
				continue;
			}
			ctx->ip += 4;
			break;
			
		case instrJmpIfNot:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			if (!a1.as_bool) {
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
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			if (a1.as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip += 4;
			break;
			
		case instrJmpAbsIfNot:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			if (!a1.as_bool) {
				INSTR_JMPABS();
				continue;
			}
			ctx->ip += 4;
			break;

        case instrCall:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			r3 = instrs[ctx->ip++];
			r4 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ct_ctx_callProcedure(ctx, a1.as_uint, r2, r3, r4);
			break;

		case instrCallStatic:
			ct_loadBytes(instrs, &ctx->ip, sizeof(u32), &u32);
			r2 = instrs[ctx->ip++];
			r3 = instrs[ctx->ip++];
			r4 = instrs[ctx->ip++];
			ct_ctx_callProcedure(ctx, u32, r2, r3, r4);
			break;

        case instrReturn:
			ct_ctx_returnProcedure(ctx, (ctAtom){.as_uint=0}, ctAtomType_NoneType);
			break;

		case instrReturnVal:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ct_ctx_returnProcedure(ctx, a1, t1);
			break;

		case instrConNew:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r2, &a1, &t1);
			a2.as_container = ct_containers_newContainer(ctx->containers, a1.as_uint);
			ct_ctx_storeAtom(ctx, r1, a2, ctAtomType_Container);
			break;
			
		case instrConDel:
			r1 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ct_containers_decRef(ctx->containers, a1.as_container);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_uint=0}, ctAtomType_NoneType);
			break;

		case instrConGet:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			r3 = instrs[ctx->ip++];

			ct_ctx_loadAtom(ctx, r2, &a1, &t1);
			ct_ctx_loadAtom(ctx, r3, &a2, &t2);

			typed_atom = ct_containers_conGet(ctx->containers, a1.as_container, a2.as_uint, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}

			ct_ctx_storeAtom(ctx, r1, typed_atom.atom, typed_atom.type);
			break;

		case instrConSet:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			r3 = instrs[ctx->ip++];

			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ct_ctx_loadAtom(ctx, r2, &a2, &t2);
			ct_ctx_loadAtom(ctx, r3, &a3, &t3);
			
			ct_containers_conSet(ctx->containers, a1.as_container, a2.as_uint, (ctTypedAtom){t3, a3}, &ctx->error);

			if (ctx->error.code != ctErrorCode_None) {
				ct_ctx_throwError(ctx, ctx->error);
				return;
			}
			break;

		case instrConLen:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r2, &a1, &t1);
			ct_ctx_storeAtom(ctx, r1, (ctAtom){.as_uint = a1.as_container->size}, ctAtomType_UInt);
			break;

		case instrConResize:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r1, &a1, &t1);
			ct_ctx_loadAtom(ctx, r2, &a2, &t2);
			ct_containers_conResize(ctx->containers, a1.as_container, a2.as_uint, &ctx->error);
			break;

		case instrConCopy:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r2, &a2, &t2);
			a1.as_container = ct_containers_conCopy(
				ctx->containers, a2.as_container, &ctx->error
			);
			ct_ctx_storeAtom(ctx, r1, a1, ctAtomType_Container);
			break;

		case instrConClone:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			ct_ctx_loadAtom(ctx, r2, &a2, &t2);
			a1.as_container = ct_containers_conClone(
				ctx->containers, a2.as_container, &ctx->error
			);
			ct_ctx_storeAtom(ctx, r1, a1, ctAtomType_Container);
			break;
	}
	};
};