#ifndef CORE_CORE_H
#define CORE_CORE_H

#include "tokenizer/tokenizer.hpp"

class ctAssemblerCore {

	ctTokenizer tokenizer;

public:

	void assemble(std::string filepath);
	
};

#endif // CORE_CORE_H