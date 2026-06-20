#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "tokens.hpp"
#include "tokenizer.hpp"



char ctTokenizer::next() {
	if (mCurrent < mSize) {
		char c = mSource.at(mCurrent++);
		return c;
	}
	return ' ';
}


char ctTokenizer::peek() {
	if (mCurrent < mSize) {
		char c = mSource.at(mCurrent);
		return c;
	}
	return ' ';
}


void ctTokenizer::backtrack() {
	mCurrent--;
}


void ctTokenizer::eatWhitspace() {

	char c;
	c = next();

	while ((c == ' ' or c == '\n' or c == '\t') and mCurrent < mSize) {
		c = next();
	}

	backtrack();
}


void ctTokenizer::tokenizeWord() {
	char c;
	uint start = mCurrent;

	while (mCurrent < mSize) {
		c = peek();

		if (std::isalnum(c) or c == '_') {
			next();
			continue;
		}
		break;
	}
	mTokens.emplace_back(ctToken(ctTokenType::Word, start, mCurrent-start));
}


void ctTokenizer::tokenizeNumber() {

	uint start = mCurrent;
	char c;
	bool is_float = false;

	while (mCurrent < mSize) {

		c = peek();

		if (std::isdigit(c)) {
			next();
			continue;
		}

		if (c == '.') {
			is_float = true;
			next();
			continue;
		}

		break;
	}

	if (is_float) {
		mTokens.emplace_back(ctToken(ctTokenType::Float, start, mCurrent-start));
	} else {
		mTokens.emplace_back(ctToken(ctTokenType::Int, start, mCurrent-start));
	}
}

void ctTokenizer::tokenizeChar() {
	char c = next();
	
	if (c != '\'') {
		// error
	}

	uint start = mCurrent;
	next(); // the char

	c = next();
	
	if (c != '\'') {
		// error
	}

	mTokens.emplace_back(ctToken(ctTokenType::Char, start, 1));
};


void ctTokenizer::tokenizeString() {
	char c = next();
	
	if (c != '\"') {
		// error
	}

	uint start = mCurrent;

	c = next();

	while (c != '\"' and mCurrent < mSize) {
		c = next();

		if (c == '\\') {
			next();
		}
	}

	mTokens.emplace_back(ctToken(ctTokenType::String, start, mCurrent-start-1));
}
	

void ctTokenizer::tokenizeSymbol() {
	char c = next();
	if (c == ' ' or c == '\n' or c == '\t') {return;}
	mTokens.emplace_back(ctToken(ctTokenType::Symbol, mCurrent-1, 1));
}



ctTokenStream ctTokenizer::tokenize(std::string source) {

	mSource = std::move(source);
	mCurrent = 0;
	mSize = mSource.size();
	mTokens = {};

	char c;

	while (mCurrent < mSize) {
		eatWhitspace();

		c = peek();

		if (std::isalpha(c) or c == '_') {
			tokenizeWord();
		}
		else if (std::isdigit(c)) {
			tokenizeNumber();
		}
		else if (c == '\'') {
			tokenizeChar();
		}
		else if (c == '\"') {
			tokenizeString();
		}
		else {
			tokenizeSymbol();
		}
	}

	mTokens.emplace_back(ctToken(ctTokenType::EndOfFile, 0 ,0));

	return ctTokenStream(std::move(mSource), std::move(mTokens));
}