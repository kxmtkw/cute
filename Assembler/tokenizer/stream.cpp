#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "tokens.hpp"
#include "tokenizer.hpp"


std::string ctTokenStream::resolveBackSlashes(const std::string& str){

	std::string resolved_str;
	char c;

	for (uint i = 0; i < str.size(); i++) {
		c = str[i];
		
		if (c != '\\') {
			resolved_str += c;
			continue;
		}

		i++;
		c = str[i];

		switch (c) {
			case '\\': resolved_str += '\\'; break;
			case '\"': resolved_str += '\"'; break;
			case '\'': resolved_str += '\''; break;
			case 'n':  resolved_str += '\n'; break;
			case 't':  resolved_str += '\t'; break;
			case 'r':  resolved_str += '\r'; break;
			case 'b':  resolved_str += '\b'; break;
			case '0':  resolved_str += '\0'; break;
			default:   resolved_str += c;    break;
		}		
	}

	return resolved_str;
}


ctToken ctTokenStream::next() {
	if (mCurrent < mTokens.size()) {
		return mTokens[mCurrent++];
	}
	return mTokens[mTokens.size()-1];
};


ctToken ctTokenStream::peek() {
	if (mCurrent < mTokens.size()) {
		return mTokens[mCurrent];
	}
	return mTokens[mTokens.size()-1];
};


void ctTokenStream::backtrack() {
	if (mCurrent > 0) mCurrent--;
};

void ctTokenStream::reset() {
	mCurrent = 0;
}

std::string ctTokenStream::getValue(ctToken& token) {

	if (token.start > mSource.size() or token.start + token.len > mSource.size()) {
		return "";
	}

	if (token.type == ctTokenType::String) {
		return resolveBackSlashes(mSource.substr(token.start, token.len));
	}

	return mSource.substr(token.start, token.len);
}


bool ctTokenStream::expectTokenType(ctTokenType type, std::string& dest) {
	ctToken token = peek();

	if (token.type != type) {
		return false;
	}

	dest = getValue(token);
	next();
	return true;
}


bool ctTokenStream::expectToken(const std::string& dest) {

	ctToken token = next();

	if (getValue(token) == dest) {
		return true;
	}
	else {
		backtrack();
		return false;
	}
	
}