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
	std::vector<ctToken> tokens = tokenizer.tokenize(&source);
	printTokenArray(tokens);

	return 0;
}