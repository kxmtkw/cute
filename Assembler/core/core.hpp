#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "resolver/resolver.hpp"
#include "codegen/generator.hpp"

class ctAssemblerCore {

	ctUtils::ctErrorCollector mErrors;

	ctTokenizer mTokenizer;
	ctParser mParser;
	ctNameResolver mResolver;
	ctCodeGenerator mCodegen;


public:

	ctAssemblerCore():
	mResolver(mErrors),
	mCodegen(mErrors) {};
	
	void assembleString(const std::string& str);
	void assembleFile(const std::string& filepath);
};

#endif // CORE_CORE_H