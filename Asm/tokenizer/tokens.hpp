
#ifndef TOKENIZER_TOKENS_H
#define TOKENIZER_TOKENS_H

#include <iostream>
#include <string>
#include <vector>

enum class ctTokenType {
	EndOfFile,
	Word,
	Int,
	Float,
	String,
	Symbol
};

struct ctToken {
	ctTokenType type;
	std::string payload;

	ctToken() = default;
	ctToken(ctTokenType t, std::string s): type(t), payload(s) {};
};



static inline std::string 
_tokenTypeToString(ctTokenType type) {
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

static inline void 
printToken(const ctToken& token) {
    std::cout << "[ " << _tokenTypeToString(token.type) << " " << token.payload << " ]\n";
}

static inline
void printTokenArray(const std::vector<ctToken>& tokens) {
    for (const auto& token : tokens) {
        printToken(token);
    }
}

#endif // TOKENIZER_TOKENS_H