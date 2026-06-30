
#ifndef CODEGEN_GENERATOR_HPP
#define CODEGEN_GENERATOR_HPP

#include <cstdint>
#include <memory>
#include <vector>

extern "C" {
	#include "CuteInstr.h"
}


#include "nodes/nodes.hpp"
#include "utils/utils.hpp"



class ctCodeGenerator : ctNodeVisitor {

	ctUtils::ctErrorCollector& mError;

	std::unique_ptr<ctImage> mImage = nullptr;
	std::vector<ctImageProcedure> mProcedures;
	std::vector<ctInstructionSize> mInstrPool;

public:

	ctCodeGenerator(ctUtils::ctErrorCollector& _error): mError(_error) {};

	void visit(ctProgramNode& node) override;
	void visit(ctProcedureNode& node) override;
	void visit(ctStationNode& node) override;
	void visit(ctOperationNode& node) override;
	void visit(ctWordNode& node) override;
	void visit(ctSlotNode& node) override;
	void visit(ctIntNode& node) override;
	void visit(ctFloatNode& node) override;

	void generate(ctProgramNode& node, std::string outfile);
};

#endif // CODEGEN_GENERATOR_HPP