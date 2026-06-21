
#ifndef TOKENIZER_TOKENS_H
#define TOKENIZER_TOKENS_H

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

enum class ctTokenType {
	EndOfFile,
	Word,
	Int,
	Float,
	String,
	Char,
	Symbol
};

struct ctToken {
	ctTokenType type;
	uint start;
	uint len;

	ctToken() = default;
	ctToken(ctTokenType t, uint s, uint l): type(t), start(s), len(l) {};
};


static inline std::string 
tokenTypeToString(ctTokenType type) {
    switch (type) {
        case ctTokenType::EndOfFile: return "EndOfFile";
        case ctTokenType::Word:      return "Word";
        case ctTokenType::Int:       return "Int";
        case ctTokenType::Float:     return "Float";
        case ctTokenType::String:    return "String";
        case ctTokenType::Symbol:    return "Symbol";
        default:                     return "Unknown";
    }
}


class ctTokenStream {

	std::string mSource;
	std::vector<ctToken> mTokens;
	uint mCurrent;

	// resolve backslashes of string tokens
	std::string resolveBackSlashes(const std::string& str);

public:

	// get the next token
	ctToken next();
	// take a look at the next token
	ctToken peek();
	// backtrack by one token
	void backtrack();
	// back to index 0
	void reset();

	// get the value of the token.
	std::string getValue(ctToken& token);

	// expect a certain token type and write its value to the string provided.
	bool expectTokenType(ctTokenType type, std::string& dest);

	// expect a certain string literal
	bool expectToken(const std::string& dest);
	
	ctTokenStream(std::string src, std::vector<ctToken> tokens): 
	mSource(std::move(src)), mTokens(std::move(tokens)), mCurrent(0) {}
};



#endif // TOKENIZER_TOKENS_H