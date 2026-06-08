
#ifndef PARSER_META_HPP
#define PARSER_META_HPP

#include "CuteInstr.h"
#include <map>
#include <string>
#include <vector>


namespace ctParsed {

	enum class InstrArgType {
		Int,
		UInt,
		Float,
		Bool,
		Char,
		Register,
		Local,
		Station
	};


	struct InstrArg {
		InstrArgType type;
		std::string payload;
	};

	struct InstrUnit {
		ctInstruction opcode;
		std::vector<InstrArg> args;
	};

	struct Procedure {
		std::vector<InstrUnit> instructions;
	};

	static const std::map<std::string, std::vector<InstrArgType>> instrMap = {
		{"halt",     {InstrArgType::Register}},
		{"null",     {}},
		{"out",      {InstrArgType::Register}},
		{"typeof",   {InstrArgType::Register, InstrArgType::Register}},
		{"mov",      {InstrArgType::Register, InstrArgType::Register}},
		{"load",     {InstrArgType::Register, InstrArgType::Local}},
		{"store",    {InstrArgType::Local,    InstrArgType::Register}},
		{"seti",     {InstrArgType::Register, InstrArgType::Int}},
		{"setu",     {InstrArgType::Register, InstrArgType::UInt}},
		{"setf",     {InstrArgType::Register, InstrArgType::Float}},
		{"setb",     {InstrArgType::Register, InstrArgType::Bool}},
		{"setc",     {InstrArgType::Register, InstrArgType::Char}},
		{"addi",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"subi",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"muli",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"divi",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"modi",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"negi",     {InstrArgType::Register, InstrArgType::Register}},
		{"absi",     {InstrArgType::Register, InstrArgType::Register}},
		{"addu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"subu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"mulu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"divu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"modu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"addf",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"subf",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"mulf",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"divf",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"negf",     {InstrArgType::Register, InstrArgType::Register}},
		{"absf",     {InstrArgType::Register, InstrArgType::Register}},
		{"and",      {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"or",       {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"not",      {InstrArgType::Register, InstrArgType::Register}},
		{"band",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"bor",      {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"bxor",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"bnot",     {InstrArgType::Register, InstrArgType::Register}},
		{"bshl",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"bshr",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"cmpi",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"cmpu",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"cmpf",     {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"eq",       {InstrArgType::Register}},
		{"neq",      {InstrArgType::Register}},
		{"less",     {InstrArgType::Register}},
		{"lesseq",   {InstrArgType::Register}},
		{"great",    {InstrArgType::Register}},
		{"greateq",  {InstrArgType::Register}},
		{"jmp",      {InstrArgType::Station}},
		{"jmpif",    {InstrArgType::Register, InstrArgType::Station}},
		{"jmpifnot", {InstrArgType::Register, InstrArgType::Station}},
		{"call",     {InstrArgType::Register}},
		{"ret",      {}},
		{"connew",   {InstrArgType::Register, InstrArgType::Register}},
		{"condel",   {InstrArgType::Register}},
		{"conget",   {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"conset",   {InstrArgType::Register, InstrArgType::Register, InstrArgType::Register}},
		{"conclone", {InstrArgType::Register, InstrArgType::Register}},
		{"conresize",{InstrArgType::Register, InstrArgType::Register}},
		{"conlen",   {InstrArgType::Register, InstrArgType::Register}},
	};

}
#endif
