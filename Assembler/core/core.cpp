#include "nodes/nodes.hpp"
#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "resolver/resolver.hpp"
#include "codegen/generator.hpp"
#include "tokenizer/tokens.hpp"

#include "core.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>

void ctAssemblerCore::assembleString(const std::string& source) {

	ctTokenStream stream = mTokenizer.tokenize(std::move(source));

	while (stream.peek().type != ctTokenType::EndOfFile) {
		auto token = stream.next();
		std::cout << tokenTypeToString(token.type) << " " << stream.getValue(token) << "\n";
	}
	stream.reset();

	auto program = mParser.parse(stream);

	ctNodePrinter printer;
	program->accept(printer);
	
	mResolver.resolve(*program);

	if (mErrors.hasError()) {
		std::cerr << mErrors.getErrors();
		return;
	}

	mCodegen.generate(*program, "asm.cti");

	if (mErrors.hasError()) {
		std::cerr << mErrors.getErrors();
		return;
	}

	std::cout << "Image written to file: asm.cti\n";
};


void ctAssemblerCore::assembleFile(const std::string& filepath) {

	std::ifstream file(filepath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string source= buffer.rdbuf()->str();
	file.close();
	assembleString(source);
};