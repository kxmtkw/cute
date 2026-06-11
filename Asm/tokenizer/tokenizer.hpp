#ifndef TOKENIZER_TOKENIZER_H
#define TOKENIZER_TOKENIZER_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "tokens.hpp"


class ctTokenizer {

	std::string mSource;
	std::vector<ctToken> mTokens;
	uint mCurrent;
	uint mSize;

	// get next char
	char next();
	// peek next char
	char peek();
	// backtrack by one char
	void backtrack();

	// eat all spaces, new lines, tabs until there is none.
	void eatWhitspace();

	// tokenize a word. a word will always start with an alphabet or underscore and can be continued using numbers.
	void tokenizeWord();
	// tokenize a number. ints and floats included. does not handle negatives
	void tokenizeNumber();
	// tokenize a symbol. usually a single character
	void tokenizeSymbol();
	// tokenize a string, backslashes are not resolved here but the TokenStream resolves them with getValue
	void tokenizeString();
	// tokenize a char, should use '', and only a single is allowed as usual
	void tokenizeChar();

public:

	// tokenize a string and return ctTokenStream object.
	ctTokenStream tokenize(std::string source);
};

#endif // TOKENIZER_TOKENIZER_H