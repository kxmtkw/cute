#include "codegen/generator.hpp"
#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "tokenizer/tokens.hpp"
#include "utils/utils.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>


int main() {

	
	std::ifstream file("asm.test");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string source= buffer.rdbuf()->str();
	file.close();

	ctTokenizer tokenizer;
	ctTokenStream stream = tokenizer.tokenize(std::move(source));

	ctParser parser;
	auto program = parser.parse(stream);

	ctUtils::ctErrorCollector errors;

	ctCodeGenerator generator(errors);
	generator.generate(*program, "asm.cti");

	if (errors.hasError()) {
		std::cerr << errors.getErrors();
		return 1;
	}

	std::cout << "Image written to file: asm.cti\n";
	return 0;
}