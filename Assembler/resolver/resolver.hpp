
#ifndef RESOLVER_NAMES_HPP
#define RESOLVER_NAMES_HPP

#include "nodes/nodes.hpp"
#include "utils/utils.hpp"
#include <map>



class ctNameResolver: ctNodeVisitor {

	ctUtils::ctErrorCollector& mError;

	std::map<std::string, ctSymbol>* mSymbolTable;
	unsigned int mProcedureCounter = 1;
	
public:

	ctNameResolver(ctUtils::ctErrorCollector& _error):
	mError(_error) {};
	
	void visit(ctProgramNode& node) override;
	void visit(ctProcedureNode& node) override;
	void visit(ctStationNode& node) override;
	void visit(ctOperationNode& node) override;
	void visit(ctWordNode& node) override;
	void visit(ctRegisterNode& node) override;
	void visit(ctSlotNode& node) override;
	void visit(ctIntNode& node) override;
	void visit(ctFloatNode& node) override;

	void resolve(ctProgramNode& node);
};

#endif // RESOLVER_NAMES_HPP