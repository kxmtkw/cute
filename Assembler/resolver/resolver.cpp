
#include "nodes/nodes.hpp"
#include "symbols/symbols.hpp"
#include <format>

#include "resolver.hpp"




void ctNameResolver::visit(ctProgramNode& node) {
	
	mSymbolTable = &node.symbolmap;
	for (auto& proc: node.procedures) {
		proc->accept(*this);
	};

}


void ctNameResolver::visit(ctProcedureNode& node) {

	if (!mFirstPass) return;

	if (mSymbolTable->contains(node.name) && mFirstPass) { 
		mError.accumulate(
			std::format("Procedure '{}' already defined.", node.name)
		);
		return;
	}
		
		
	for (auto& op: node.operations) {
		op->accept(*this);
	};


	if (node.name == "0") {
		node.assigned_id = 0;
	} else {
		node.assigned_id = mProcedureCounter++;
	};

	mSymbolTable->emplace(node.name, ctSymbol(ctSymbolType::Procedure, node.name, &node));
}


void ctNameResolver::visit(ctOperationNode& node) {

}


void ctNameResolver::visit(ctWordNode& node) {

	if (mFirstPass) return;

	std::cout << "TFFF" << "\n";

	if (!mSymbolTable->contains(node.val)) {
		mError.accumulate(
			std::format("Unknown symbol '{}'", node.val)
		);
		return;
	}

	node.sym = &mSymbolTable->at(node.val);
}


void ctNameResolver::visit(ctSlotNode& node) {
}


void ctNameResolver::visit(ctIntNode& node) {
}


void ctNameResolver::visit(ctFloatNode& node) {
}


void ctNameResolver::resolve(ctProgramNode& node) {
	mFirstPass = true;
	node.accept(*this);
	mFirstPass = false;
	node.accept(*this);
}

