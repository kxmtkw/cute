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
#include "CuteByte.h"
}

struct InstrSpec {
	CtInstruction opcode;
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
parseRegisterOrByte(const std::string& token)
{
	std::string text = token;
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

static uint32_t
parseU32(const std::string& token)
{
	long long value = 0;
	try {
		value = std::stoll(token, nullptr, 0);
	} catch (const std::exception&) {
		throw std::runtime_error("invalid 4-byte integer argument: " + token);
	}

	return static_cast<uint32_t>(static_cast<int32_t>(value));
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

static std::vector<uint8_t>
assemble(const std::vector<std::string>& tokens)
{
	std::vector<uint8_t> bytecode;

	for (size_t i = 0; i < tokens.size();) {
		const std::string& mnemonic = tokens[i++];
		const auto specIt = instrMap.find(mnemonic);
		if (specIt == instrMap.end()) {
			throw std::runtime_error("unknown instruction: " + mnemonic);
		}

		const InstrSpec& spec = specIt->second;
		appendU8(bytecode, static_cast<uint8_t>(spec.opcode));

		for (size_t argIndex = 0; argIndex < spec.arg_lengths.size(); ++argIndex) {
			if (i >= tokens.size()) {
				throw std::runtime_error("missing argument for instruction: " + mnemonic);
			}

			const std::string& arg = tokens[i++];
			const size_t argLength = spec.arg_lengths[argIndex];

			if (argLength == 1) {
				appendU8(bytecode, parseRegisterOrByte(arg));
				continue;
			}

			if (argLength == 4) {
				if (mnemonic == "loadf") {
					appendU32(bytecode, parseF32Bits(arg));
				} else {
					appendU32(bytecode, parseU32(arg));
				}
				continue;
			}

			throw std::runtime_error("unsupported argument width in instruction map: " + mnemonic);
		}
	}

	return bytecode;
}

static std::string
imageCodeToString(CtImageCode code)
{
	switch (code) {
	case CtImageCode_Success:
		return "success";
	case CtImageCode_FileNotFound:
		return "file not found";
	case CtImageCode_ReadWriteFailure:
		return "read/write failure";
	case CtImageCode_InvalidImage:
		return "invalid image";
	}

	return "unknown image error";
}

static void
writeImage(const std::string& path, const std::vector<uint8_t>& bytes)
{
	CtImage img;
	CtImage_init(&img);

	img.header.procedure_count = 1;
	img.header.instruction_count = bytes.size();

	img.procedure_table = static_cast<CtImageProcedure*>(std::malloc(sizeof(CtImageProcedure)));
	if (img.procedure_table == nullptr) {
		CtImage_del(&img);
		throw std::runtime_error("failed to allocate procedure table");
	}

	img.instruction_pool = static_cast<CtInstructionSize*>(std::malloc(bytes.size() * sizeof(CtInstructionSize)));
	if (img.instruction_pool == nullptr && !bytes.empty()) {
		CtImage_del(&img);
		throw std::runtime_error("failed to allocate instruction pool");
	}

	img.procedure_table[0].id = 0;
	img.procedure_table[0].bytecode_index = 0;
	img.procedure_table[0].locals_size = 0;

	for (size_t i = 0; i < bytes.size(); ++i) {
		img.instruction_pool[i] = bytes[i];
	}

	const CtImageCode code = CtImage_write(&img, path.c_str());
	CtImage_del(&img);

	if (code != CtImageCode_Success) {
		throw std::runtime_error("failed to write CtImage: " + imageCodeToString(code));
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
		const std::vector<uint8_t> bytecode = assemble(tokens);

		writeImage(outputPath, bytecode);
		//std::cout << "Wrote CtImage with " << bytecode.size() << " instruction bytes to " << outputPath << '\n';
		return 0;
	} catch (const std::exception& ex) {
		std::cerr << "assembler error: " << ex.what() << '\n';
		return 1;
	}
}
