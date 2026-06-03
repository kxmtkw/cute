#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

extern "C" {
#include "CuteInstr.h"
#include "CuteConfig.h"
}

struct InstrSpec {
	ctInstruction opcode;
	std::vector<size_t> arg_lengths;
};

static const std::map<std::string, InstrSpec> instrMap = {
	{"halt", {instrHalt, {1}}},
	{"null", {instrNull, {}}},
	{"out", {instrOut, {1, 1}}},
	{"typeof", {instrTypeOf, {1}}},
	{"mov", {instrMov, {1, 1}}},
	{"loadi", {instrLoadI, {1, 4}}},
	{"loadu", {instrLoadU, {1, 4}}},
	{"loadf", {instrLoadF, {1, 4}}},
	{"loadb", {instrLoadB, {1, 4}}},
	{"loadc", {instrLoadC, {1, 4}}},
	{"addi", {instrAddI, {1, 1}}},
	{"subi", {instrSubI, {1, 1}}},
	{"muli", {instrMulI, {1, 1}}},
	{"divi", {instrDivI, {1, 1}}},
	{"modi", {instrModI, {1, 1}}},
	{"negi", {instrNegI, {1}}},
	{"addu", {instrAddU, {1, 1}}},
	{"subu", {instrSubU, {1, 1}}},
	{"mulu", {instrMulU, {1, 1}}},
	{"divu", {instrDivU, {1, 1}}},
	{"modu", {instrModU, {1, 1}}},
	{"addf", {instrAddF, {1, 1}}},
	{"subf", {instrSubF, {1, 1}}},
	{"mulf", {instrMulF, {1, 1}}},
	{"divf", {instrDivF, {1, 1}}},
	{"negf", {instrNegF, {1}}},
	{"and", {instrLogicAnd, {1, 1}}},
	{"or", {instrLogicOr, {1, 1}}},
	{"not", {instrLogicNot, {1}}},
	{"band", {instrBitAnd, {1, 1}}},
	{"bor", {instrBitOr, {1, 1}}},
	{"bxor", {instrBitXor, {1, 1}}},
	{"bnot", {instrBitNot, {1,}}},
	{"bshl", {instrBitShl, {1, 1}}},
	{"bshr", {instrBitShr, {1, 1}}},
	{"cmpi", {instrCmpI, {1, 1}}},
	{"cmpu", {instrCmpU, {1, 1}}},
	{"cmpf", {instrCmpF, {1, 1}}},
	{"eq", {instrEq, {1}}},
	{"neq", {instrNotEq, {1}}},
	{"less", {instrLess, {1}}},
	{"lesseq", {instrLessEq, {1}}},
	{"great", {instrGreater, {1}}},
	{"greateq", {instrGreaterEq, {1}}},
	{"jmp", {instrJmp, {1}}},
	{"jmpif", {instrJmpIf, {1, 1}}},
	{"jmpifnot", {instrJmpIfNot, {1, 1}}},
	{"jma", {instrJmpAbs, {1}}},
	{"jmaif", {instrJmpAbsIf, {1, 1}}},
	{"jmaifnot", {instrJmpAbsIfNot, {1, 1}}},
	{"call", {instrCall, {4}}},
	{"ret", {instrReturn, {}}},
	{"connew", {instrConNew, {1}}},
	{"condel", {instrConDel, {1}}},
	{"conget", {instrConGet, {1, 1}}},
	{"conset", {instrConSet, {1, 1, 1}}},
};

static std::string
defaultOutputPath(const std::string& inputPath)
{
	return inputPath + ".img";
}

static std::string
readFile(const std::string& path)
{
	std::ifstream in(path);
	if (!in) {
		throw std::runtime_error("failed to open input file: " + path);
	}

	return std::string(
		std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>()
	);
}

static std::vector<std::string>
tokenize(const std::string& source)
{
	std::istringstream stream(source);
	std::vector<std::string> tokens;
	std::string token;

	while (stream >> token) {
		tokens.push_back(token);
	}

	return tokens;
}

static void
appendU8(std::vector<uint8_t>& out, uint8_t value)
{
	out.push_back(value);
}

static void
appendU32(std::vector<uint8_t>& out, uint32_t value)
{
	for (size_t i = 0; i < 4; ++i) {
		out.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFFu));
	}
}

static uint8_t
parseRegisterOrByte(const std::string& token, uint32_t& maxLocalUsed)
{
	std::string text = token;

	// Handle local variables: s0, s1, s2, ...
	if (!text.empty() && (text[0] == 's' || text[0] == 'S')) {
		text.erase(text.begin());
		unsigned long localIndex = 0;
		try {
			localIndex = std::stoul(text, nullptr, 0);
		} catch (const std::exception&) {
			throw std::runtime_error("invalid local variable: " + token);
		}

		if (localIndex >= CUTE_CONF_LOCALS_LIMIT) {
			throw std::runtime_error("local index out of range (max " + std::to_string(CUTE_CONF_LOCALS_LIMIT - 1) + "): " + token);
		}

		if (localIndex > maxLocalUsed) {
			maxLocalUsed = localIndex;
		}

		return static_cast<uint8_t>(CUTE_CONF_REGISTER_COUNT + localIndex);
	}

	// Handle registers: r0, r1, ..., or bare numbers
	if (!text.empty() && (text[0] == 'r' || text[0] == 'R')) {
		text.erase(text.begin());
	}

	unsigned long value = 0;
	try {
		value = std::stoul(text, nullptr, 0);
	} catch (const std::exception&) {
		throw std::runtime_error("invalid 1-byte argument: " + token);
	}

	if (value > std::numeric_limits<uint8_t>::max()) {
		throw std::runtime_error("1-byte argument out of range: " + token);
	}

	return static_cast<uint8_t>(value);
}

static int32_t
parseU32(const std::string& token)
{
	long long value = 0;
	try {
		value = std::stoll(token, nullptr, 0);
	} catch (const std::exception&) {
		throw std::runtime_error("invalid 4-byte integer argument: " + token);
	}

	return value;
}

static uint32_t
parseF32Bits(const std::string& token)
{
	float value = 0.0f;
	try {
		value = std::stof(token);
	} catch (const std::exception&) {
		throw std::runtime_error("invalid 4-byte float argument: " + token);
	}

	uint32_t bits = 0;
	static_assert(sizeof(bits) == sizeof(value));
	std::memcpy(&bits, &value, sizeof(bits));
	return bits;
}

struct Procedure {
	std::string name;
	uint64_t id;
	uint64_t bytecode_index;
	uint32_t locals_size;
};

static std::vector<uint8_t>
assemble(const std::vector<std::string>& tokens, std::vector<Procedure>& procedures)
{
	std::vector<uint8_t> bytecode;
	std::map<uint64_t, bool> procedureIds; // track used numeric procedure ids

	for (size_t i = 0; i < tokens.size();) {
		const std::string& token = tokens[i];

		// Check if this is a procedure definition (name [ instructions... ])
		if (i + 1 < tokens.size() && tokens[i + 1] == "[") {
			// find matching closing ]
			size_t p = i + 2;
			while (p < tokens.size() && tokens[p] != "]") {
				++p;
			}
			if (p >= tokens.size() || tokens[p] != "]") {
				throw std::runtime_error("unterminated procedure block for: " + token);
			}

			// parse procedure id (must be numeric). user requested syntax: id [ instructions ]
			uint64_t procId = 0;
			try {
				procId = std::stoull(token, nullptr, 0);
			} catch (const std::exception&) {
				throw std::runtime_error("procedure id must be numeric: " + token);
			}

			if (procedureIds.find(procId) != procedureIds.end()) {
				throw std::runtime_error("procedure id already defined: " + token);
			}

			Procedure proc;
			proc.name = "";
			proc.id = procId;
			proc.bytecode_index = bytecode.size();
			proc.locals_size = 0;

			uint32_t maxLocalUsed = 0;

			procedures.push_back(proc);
			procedureIds[procId] = true;

			// assemble instructions inside the block [ ... ]
			size_t inner = i + 2;
			while (inner < p) {
				const std::string& innerTok = tokens[inner];
				const auto specItInner = instrMap.find(innerTok);
				if (specItInner == instrMap.end()) {
					throw std::runtime_error("unknown instruction in procedure '" + token + "': " + innerTok);
				}

				const InstrSpec& specInner = specItInner->second;
				appendU8(bytecode, static_cast<uint8_t>(specInner.opcode));

				++inner;

				for (size_t argIndex = 0; argIndex < specInner.arg_lengths.size(); ++argIndex) {
					if (inner >= p) {
						throw std::runtime_error("missing argument for instruction: " + innerTok);
					}

					const std::string& arg = tokens[inner++];
					const size_t argLength = specInner.arg_lengths[argIndex];

					if (argLength == 1) {
						appendU8(bytecode, parseRegisterOrByte(arg, maxLocalUsed));
						continue;
					}

					if (argLength == 4) {
						if (innerTok == "loadf") {
							appendU32(bytecode, parseF32Bits(arg));
						} else {
							appendU32(bytecode, parseU32(arg));
						}
						continue;
					}

					throw std::runtime_error("unsupported argument width in instruction map: " + innerTok);
				}
			}

			// set locals_size based on highest local used
			procedures.back().locals_size = maxLocalUsed + 1;

			// advance i past the entire procedure block (name [ ... ])
			i = p + 1;
			continue;
		}

		// Otherwise, parse as instruction (top-level instructions shouldn't use locals)
		const auto specIt = instrMap.find(token);
		if (specIt == instrMap.end()) {
			throw std::runtime_error("unknown instruction: " + token);
		}

		const InstrSpec& spec = specIt->second;
		appendU8(bytecode, static_cast<uint8_t>(spec.opcode));

		++i;

		uint32_t dummyMaxLocal = 0;
		for (size_t argIndex = 0; argIndex < spec.arg_lengths.size(); ++argIndex) {
			if (i >= tokens.size()) {
				throw std::runtime_error("missing argument for instruction: " + token);
			}

			const std::string& arg = tokens[i++];
			const size_t argLength = spec.arg_lengths[argIndex];

			if (argLength == 1) {
				appendU8(bytecode, parseRegisterOrByte(arg, dummyMaxLocal));
				continue;
			}

			if (argLength == 4) {
				if (token == "loadf") {
					appendU32(bytecode, parseF32Bits(arg));
				} else {
					appendU32(bytecode, parseU32(arg));
				}
				continue;
			}

			throw std::runtime_error("unsupported argument width in instruction map: " + token);
		}
	}

	if (procedures.empty()) {
		throw std::runtime_error("no procedures defined");
	}

	return bytecode;
}

static std::string
imageCodeToString(ctImageCode code)
{
	switch (code) {
	case ctImageCode_Success:
		return "success";
	case ctImageCode_FileNotFound:
		return "file not found";
	case ctImageCode_ReadWriteFailure:
		return "read/write failure";
	case ctImageCode_InvalidImage:
		return "invalid image";
	}

	return "unknown image error";
}

static void
writeImage(const std::string& path, const std::vector<uint8_t>& bytes, const std::vector<Procedure>& procedures)
{
	ctImage img;

	img.header.procedure_count = procedures.size();
	img.header.instruction_count = bytes.size();

	img.procedure_table = static_cast<ctImageProcedure*>(std::malloc(procedures.size() * sizeof(ctImageProcedure)));
	if (img.procedure_table == nullptr) {
		ct_image_free(&img);
		throw std::runtime_error("failed to allocate procedure table");
	}

	img.instruction_pool = static_cast<ctInstructionSize*>(std::malloc(bytes.size() * sizeof(ctInstructionSize)));
	if (img.instruction_pool == nullptr && !bytes.empty()) {
		ct_image_free(&img);
		throw std::runtime_error("failed to allocate instruction pool");
	}

	for (size_t i = 0; i < procedures.size(); ++i) {
		img.procedure_table[i].id = procedures[i].id;
		img.procedure_table[i].bytecode_index = procedures[i].bytecode_index;
		img.procedure_table[i].locals_count = procedures[i].locals_size;
	}

	for (size_t i = 0; i < bytes.size(); ++i) {
		img.instruction_pool[i] = bytes[i];
	}

	const ctImageCode code = ct_image_write(&img, path.c_str());
	ct_image_free(&img);

	if (code != ctImageCode_Success) {
		throw std::runtime_error("failed to write ctImage: " + imageCodeToString(code));
	}
}

int
main(int argc, char** argv)
{
	if (argc < 2 || argc > 3) {
		std::cerr << "usage: asm <input-file> [output-file]\n";
		return 1;
	}

	try {
		const std::string inputPath = argv[1];
		const std::string outputPath = argc == 3 ? argv[2] : defaultOutputPath(inputPath);

		const std::string source = readFile(inputPath);
		const std::vector<std::string> tokens = tokenize(source);
		std::vector<Procedure> procedures;
		const std::vector<uint8_t> bytecode = assemble(tokens, procedures);

		writeImage(outputPath, bytecode, procedures);
		std::cout << "Wrote ctImage with " << bytecode.size() << " instruction bytes to " << outputPath << '\n';
		return 0;
	} catch (const std::exception& ex) {
		std::cerr << "assembler error: " << ex.what() << '\n';
		return 1;
	}
}
