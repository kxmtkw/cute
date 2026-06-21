#include "codegen/generator.hpp"
#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "tokenizer/tokens.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>


int main() {

	std::string source;
	
	std::ifstream file("../asm.test");

	std::stringstream buffer;
	buffer << file.rdbuf();
	source = buffer.rdbuf()->str();

	file.close();
	ctTokenizer tokenizer;
	ctTokenStream stream = tokenizer.tokenize(std::move(source));

	while (stream.peek().type != ctTokenType::EndOfFile) {
		auto tok = stream.next();
		std::cout << tokenTypeToString(tok.type) << " " << stream.getValue(tok) << "\n";
	}
	stream.reset();
	ctParser parser;
	auto program = parser.parse(stream);

	ctNodePrinter printer;
	program->accept(printer);

	ctCodeGenerator generator;
	generator.generate(*program, "../asm.cti");
	return 0;
}