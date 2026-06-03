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


static inline uint32_t
load32(ctInstructionSize* instrs, uint64_t* ip) {
	uint32_t u;
	memcpy(&u, &instrs[*ip], sizeof(u));
	*ip += 4;
	return u;
}

static inline void 
out(ctAtom atom, ctAtomTypeSize type, uint32_t fmt) {

	if (fmt == 0) {
		
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

	} else if (fmt == 1) {
		for (int i = 63; i >= 0; i--)
			putchar((atom.raw >> i) & 1 ? '1' : '0');
		putchar('\n');
	} else { 
		printf("Unknown format: %d\n", fmt);
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


static inline void
write_atom(ctContext* ctx, uint i, ctAtom atom, ctAtomTypeSize type) {

	ctRegisterFile* registers = &ctx->registers;
	ctAtomFile* atomfile = &ctx->current_frame->locals;

	if (type == ctAtomType_Container) {
		ct_containers_incRef(ctx->containers, atom.as_container);
	}

	if (i < CUTE_CONF_REGISTER_COUNT) {
		if (registers->types[i] == ctAtomType_Container) {
			ct_containers_decRef(ctx->containers, registers->atoms[i].as_container);
		}
		registers->atoms[i] = atom;
		registers->types[i] = type;
		return;
	}
	
	i = i - CUTE_CONF_REGISTER_COUNT;

	if (i < atomfile->size) {
		if (atomfile->types[i] == ctAtomType_Container) {
			ct_containers_decRef(ctx->containers, atomfile->atoms[i].as_container);
		}

		atomfile->atoms[i] = atom;
		atomfile->types[i] = type;
	}
}


static inline void
read_atom(ctContext* ctx, uint i, ctAtom* atom, ctAtomTypeSize* type) {

	ctRegisterFile* registers = &ctx->registers;
	ctAtomFile* atomfile = &ctx->current_frame->locals;

	if (i < CUTE_CONF_REGISTER_COUNT) {
		*atom = registers->atoms[i];
		*type = registers->types[i];
		return;
	}

	i = i - CUTE_CONF_REGISTER_COUNT;

	if (i < atomfile->size) {
		*atom = atomfile->atoms[i];
		*type = atomfile->types[i];
	}
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

#define CONTAINER_CHECKS(Index, Function) \
if (ctx->registers.types[Index] == ctAtomType_Container) { \
	Function(ctx->containers, ctx->registers.atoms[Index].as_container); \
} 

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
	ctContainer* con;

	ctTypedAtom typed;

	while(ctx->running)
	{
	ctInstruction instr = instrs[ctx->ip++];
	CUTE_LOG("trace", "ip: %08lu | Opcode: 0x%X | ctx: %p\n", ctx->ip-1, instr, ctx);
	
	
		switch (instr) 
		{

		case instrHalt:
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
			printf("%s\n", ct_atom_stringforms[ctx->registers.types[r1]]);
			break;

		case instrMov:
			r1 = instrs[ctx->ip++];
			r2 = instrs[ctx->ip++];
			read_atom(ctx, r2, &typed.atom, &typed.type);
			write_atom(ctx, r1, typed.atom, typed.type);
			break;

		case instrLoadI:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&i, &u, sizeof(i));
			typed.atom.as_int = i;
			typed.type = ctAtomType_Int;
			write_atom(ctx, r1, typed.atom, typed.type);
			break;

		case instrLoadU:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			typed.atom.as_uint = u;
			typed.type = ctAtomType_UInt;
			write_atom(ctx, r1, typed.atom, typed.type);
			break;

		case instrLoadF:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			memcpy(&f, &u, sizeof(f));
			typed.atom.as_float = f;
			typed.type = ctAtomType_Float;
			write_atom(ctx, r1, typed.atom, typed.type);
			break;

		case instrLoadB:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			typed.atom.as_bool = u;
			typed.type = ctAtomType_Bool;
			write_atom(ctx, r1, typed.atom, typed.type);
			break;

		case instrLoadC:
			r1 = instrs[ctx->ip++];
			u = load32(instrs, &ctx->ip);
			typed.atom.as_char = u;
			typed.type = ctAtomType_Char;
			write_atom(ctx, r1, typed.atom, typed.type);
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
			check_type(&ctx->registers, r1, ctAtomType_UInt);
			u = ctx->registers.atoms[r1].as_uint;
			con = ct_containers_newContainer(ctx->containers, u);
			ctx->registers.atoms[r1].as_container = con;
			ctx->registers.types[r1] = ctAtomType_Container;
			ct_containers_incRef(ctx->containers, con);
			break;
			
		case instrConDel:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_Container);
			con = ctx->registers.atoms[r1].as_container;
			ct_containers_delContainer(ctx->containers, con);
			ctx->registers.types[r1] = ctAtomType_NoneType;
			break;

		case instrConGet:
			r1 = instrs[ctx->ip++];
			check_type(&ctx->registers, r1, ctAtomType_Container);
			r2 = instrs[ctx->ip++];
			check_type(&ctx->registers, r2, ctAtomType_UInt);
			con = ctx->registers.atoms[r1].as_container;
			
			ct_containers_conGet(ctx->containers, con, ctx->registers.atoms[r2].as_uint, &typed);

			ct_containers_decRef(ctx->containers, con);
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
			ct_containers_conSet(ctx->containers, con, ctx->registers.atoms[r2].as_uint, typed);
			break;

		case instrConClone:
		case instrConExtend:
		case instrConShrink:
			break;

		default:
			ct_ctx_reportFailure(ctx, (ctFailure){"Illegal Instruction."});
		}
	}

}
