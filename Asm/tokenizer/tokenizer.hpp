#ifndef TOKENIZER_TOKENIZER_H
#define TOKENIZER_TOKENIZER_H

#include <cstdint>
#include <string>
#include <vector>

#include "tokens.hpp"


class ctTokenizer {

	std::string* mSource;
	std::vector<ctToken> mTokens;
	uint32_t mIndex;
	uint32_t mSize;

	char next();
	char peek();
	void backtrack();

	void eatWhitspace();

	void tokenizeWord();
	void tokenizeNumber();
	void tokenizeString();
	void tokenizeSymbol();

public:

	std::vector<ctToken> tokenize(std::string* source);
};

#endif // TOKENIZER_TOKENIZER_H