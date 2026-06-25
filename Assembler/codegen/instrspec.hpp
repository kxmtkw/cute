
#ifndef CODEGEN_INSTRSPEC_HPP
#define CODEGEN_INSTRSPEC_HPP

#include <cstddef>
#include <map>
#include <string>
#include <vector>

extern "C" {
	#include "CuteInstr.h"
}

enum class ctOperandType {
	Int,
	UInt,
	Float,
	Char,
	Bool,
	Register,
	Slot
};

static size_t ctOperandSize[] = {
	4, 4, 4, 1, 1
};
	
struct ctInstrSpec {
	ctInstruction opcode;
	std::vector<ctOperandType> operands;
};

static const std::map<std::string, ctInstrSpec> ctInstrMap = {

	{"halt",    {instrHalt,    {ctOperandType::Register}}},
    {"null",    {instrNull,    {}}},
    {"out",     {instrOut,     {ctOperandType::Register}}},
    {"typeof",  {instrTypeof,  {ctOperandType::Register, ctOperandType::Register}}},
	{"mov",     {instrMov,     {ctOperandType::Register, ctOperandType::Register}}},
	{"load",    {instrLoad,    {ctOperandType::Register, ctOperandType::Slot}}},
    {"store",   {instrStore,   {ctOperandType::Slot, ctOperandType::Register}}},

    {"seti",    {instrSetI,    {ctOperandType::Register, ctOperandType::Int}}},
    {"setu",    {instrSetU,    {ctOperandType::Register, ctOperandType::UInt}}},
    {"setf",    {instrSetF,    {ctOperandType::Register, ctOperandType::Float}}},
    {"setb",    {instrSetB,    {ctOperandType::Register, ctOperandType::Bool}}},
    {"setc",    {instrSetC,    {ctOperandType::Register, ctOperandType::Char}}},
	{"setn",    {instrSetN,    {ctOperandType::Register}}},

    {"addi",    {instrAddI,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"subi",    {instrSubI,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"muli",    {instrMulI,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"divi",    {instrDivI,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"modi",    {instrModI,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"negi",    {instrNegI,    {ctOperandType::Register, ctOperandType::Register}}},
    {"absi",    {instrAbsI,    {ctOperandType::Register, ctOperandType::Register}}},
    
    {"addu",    {instrAddU,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"subu",    {instrSubU,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"mulu",    {instrMulU,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"divu",    {instrDivU,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"modu",    {instrModU,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},

    {"addf",    {instrAddF,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"subf",    {instrSubF,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"mulf",    {instrMulF,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"divf",    {instrDivF,    {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"negf",    {instrNegF,    {ctOperandType::Register, ctOperandType::Register}}},
    {"absf",    {instrAbsF,    {ctOperandType::Register, ctOperandType::Register}}},

	{"and",     {instrLogicAnd,{ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"or",      {instrLogicOr, {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"not",     {instrLogicNot,{ctOperandType::Register, ctOperandType::Register}}},
    {"band",    {instrBitAnd,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"bor",     {instrBitOr,   {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"bxor",    {instrBitXor,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"bnot",    {instrBitNot,  {ctOperandType::Register, ctOperandType::Register}}},
    {"bshl",    {instrBitShl,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"bshr",    {instrBitShr,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},

    {"cmpi",    {instrCmpI,    {ctOperandType::Register, ctOperandType::Register}}},
    {"cmpu",    {instrCmpU,    {ctOperandType::Register, ctOperandType::Register}}},
    {"cmpf",    {instrCmpF,    {ctOperandType::Register, ctOperandType::Register}}},
    {"eq",      {instrEq,      {ctOperandType::Register}}},
    {"neq",     {instrNotEq,   {ctOperandType::Register}}},
    {"less",    {instrLess,    {ctOperandType::Register}}},
    {"lesseq",  {instrLessEq,  {ctOperandType::Register}}},
    {"great",   {instrGreater, {ctOperandType::Register}}},
    {"greateq", {instrGreaterEq,{ctOperandType::Register}}},
    
    {"jmp",     {instrJmp,     {ctOperandType::Int}}},
    {"jmpif",   {instrJmpIf,   {ctOperandType::Register, ctOperandType::Int}}},
    {"jmpifnot",{instrJmpIfNot,{ctOperandType::Register, ctOperandType::Int}}},
    {"jma",     {instrJmpAbs,  {ctOperandType::UInt}}},
    {"jmaif",   {instrJmpAbsIf,{ctOperandType::Register, ctOperandType::UInt}}},
    {"jmaifnot",{instrJmpAbsIfNot,{ctOperandType::Register, ctOperandType::UInt}}},
    {"call",    {instrCall,    {ctOperandType::Register}}},
    {"ret",     {instrReturn,  {}}},

    {"connew",   {instrConNew,  {ctOperandType::Register, ctOperandType::Register}}},
    {"condel",   {instrConDel,  {ctOperandType::Register}}},
    {"conget",   {instrConGet,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"conset",   {instrConSet,  {ctOperandType::Register, ctOperandType::Register, ctOperandType::Register}}},
    {"conresize",{instrConResize,{ctOperandType::Register, ctOperandType::Register}}},
    {"conlen",   {instrConLen,  {ctOperandType::Register, ctOperandType::Register}}},
	{"concopy",  {instrConCopy,{ctOperandType::Register, ctOperandType::Register}}},
	{"conclone", {instrConClone,{ctOperandType::Register, ctOperandType::Register}}},

};

#endif // CODEGEN_INSTRSPEC_HPP