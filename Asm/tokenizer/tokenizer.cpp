#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

#include "tokens.hpp"
#include "tokenizer.hpp"



char ctTokenizer::next() {
	if (mIndex < mSize) {
		char c = mSource->at(mIndex++);
		return c;
	}
	return ' ';
}


char ctTokenizer::peek() {
	if (mIndex < mSize) {
		char c = mSource->at(mIndex);
		return c;
	}
	return ' ';
}


void ctTokenizer::backtrack() {
	mIndex--;
}


void ctTokenizer::eatWhitspace() {

	char c;
	c = next();

	while ((c == ' ' or c == '\n' or c == '\t') and mIndex < mSize) {
		c = next();
	}

	backtrack();
}


void ctTokenizer::tokenizeWord() {
	char c;
	std::string word;

	while (true) {
		c = peek();

		if (std::isalnum(c) or c == '_') {
			word.push_back(c);
			next();
			continue;
		}
		break;
	}

	mTokens.emplace_back(ctToken(ctTokenType::Word, word));
}


void ctTokenizer::tokenizeNumber() {

	char c;
	std::string word;
	bool is_float = false;

	while (true) {

		c = peek();

		if (std::isdigit(c)) {
			word.push_back(c);
			next();
			continue;
		}

		if (c == '.') {
			is_float = true;
			word.push_back(c);
			next();
			continue;
		}

		break;
	}

	if (is_float) {
		mTokens.emplace_back(ctToken(ctTokenType::Float, word));
	} else {
		mTokens.emplace_back(ctToken(ctTokenType::Int, word));
	}
}


void ctTokenizer::tokenizeSymbol() {
	std::string s;
	s.push_back(next());
	mTokens.emplace_back(ctToken(ctTokenType::Symbol, s));
}

void ctTokenizer::tokenizeString() {}


std::vector<ctToken> ctTokenizer::tokenize(std::string* source) {

	mSource = source;
	mTokens = {};
	mIndex = 0;
	mSize = source->size();

	char c;

	while (mIndex < mSize) {
		
		eatWhitspace();

		c = peek();

		if (std::isalpha(c)) {
			tokenizeWord();
		}
		else if (std::isdigit(c)) {
			tokenizeNumber();
		}
		else {
			tokenizeSymbol();
		}
	}

	ctToken tok(ctTokenType::EndOfFile, "");
	mTokens.push_back(tok);

	return mTokens;
}