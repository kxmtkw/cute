#include "tokenizer/tokenizer.hpp"
#include "tokenizer/tokens.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


int main() {

	std::string source;
	
	std::ifstream file("asm.test");

	std::stringstream buffer;
	buffer << file.rdbuf();
	source = buffer.rdbuf()->str();

	file.close();

	ctTokenizer tokenizer;
	ctTokenStream stream = tokenizer.tokenize(std::move(source));

	while (stream.peek().type != ctTokenType::EndOfFile) {
		auto token = stream.next();
		std::cout << tokenTypeToString(token.type) << " " << stream.getValue(token) << "\n";
	}
	return 0;
}