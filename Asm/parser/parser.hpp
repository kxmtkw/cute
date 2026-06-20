#ifndef PARSER_PARSER_HPP
#define PARSER_PARSER_HPP

#include "nodes.hpp"
#include "tokenizer/tokens.hpp"
#include <memory>


class ctParser {

	ctTokenStream* mStream;

	std::unique_ptr<ctProcedureNode>
	parseProcedure();

	std::unique_ptr<ctOperationNode>
	parseOperation();

public:

	std::unique_ptr<ctProgramNode>
	parse(ctTokenStream& stream);
};

#endif // PARSER_PARSER_HPP