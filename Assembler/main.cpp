#include "codegen/generator.hpp"
#include "core/core.hpp"
#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "tokenizer/tokens.hpp"
#include "utils/utils.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>


int main() {
	ctAssemblerCore assembler;
	assembler.assembleFile("asm.test");
	return 0;
}