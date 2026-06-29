
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

	if (mSymbolTable->contains(node.name)) {
		mError.accumulate(
			std::format("Procedure '{}' already defined.", node.name)
		);
		return;
	}
		
		
	for (auto& op: node.operations) {
		op->accept(*this);
	};


	if (node.name == "main") {
		node.assigned_id = 0;
	} else {
		node.assigned_id = mProcedureCounter++;
	};

	mSymbolTable->emplace(node.name, ctSymbol(ctSymbolType::Procedure, node.name, &node));
}

void ctNameResolver::visit(ctStationNode& node) {
	
}

void ctNameResolver::visit(ctOperationNode& node) {

}


void ctNameResolver::visit(ctWordNode& node) {

}


void ctNameResolver::visit(ctRegisterNode& node) {
}


void ctNameResolver::visit(ctSlotNode& node) {
}


void ctNameResolver::visit(ctIntNode& node) {
}


void ctNameResolver::visit(ctFloatNode& node) {
}


void ctNameResolver::resolve(ctProgramNode& node) {
	node.accept(*this);
}

