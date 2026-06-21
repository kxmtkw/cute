#include <cstdint>
#include <memory>

#include "parser/nodes.hpp"
#include "generator.hpp"
#include "instrspec.hpp"
#include "utils/utils.hpp"


void ctCodeGenerator::visit(ctProgramNode& node) {

	for (auto& procedure: node.procedures) {
		procedure->accept(*this);
	}

	mImage->header.instruction_count = mInstrPool.size();
	mImage->header.procedure_count = mProcedures.size();
	mImage->procedure_table = mProcedures.data();
	mImage->instruction_pool = mInstrPool.data();

	std::cout << mInstrPool.size() << "\n";
}


void ctCodeGenerator::visit(ctProcedureNode& node) {
	ctImageProcedure image_procedure;
	image_procedure.id = node.id;
	image_procedure.bytecode_index = mInstrPool.size();
	image_procedure.locals_count = 4;
	
	for (auto& op: node.operations) {
		op->accept(*this);
	}

	mProcedures.push_back(image_procedure);
};


void ctCodeGenerator::visit(ctOperationNode& node) {
	
	ctInstrSpec spec = ctInstrMap.at(node.opcode);
	mInstrPool.push_back(spec.opcode);

	for (auto& operand: node.operands) {
		operand->accept(*this);
	}
};


void ctCodeGenerator::visit(ctWordNode& node) {}

void ctCodeGenerator::visit(ctRegisterNode& node) {
	
	uint32_t out;
	ctUtils::isRegister(node.val, out);
	uint8_t register_val = out;

	mInstrPool.push_back(register_val);
};

void ctCodeGenerator::visit(ctSlotNode& node) {
	
	uint32_t out;
	ctUtils::isSlot(node.val, out);
	uint8_t slot_val = out;

	mInstrPool.push_back(slot_val);
}


void ctCodeGenerator::visit(ctIntNode& node) {
	int32_t i;
	ctUtils::stringToInt(node.val, i);
	ctUtils::insertInt32(i, mInstrPool);
}

void ctCodeGenerator::visit(ctFloatNode& node) {
	float f;
	ctUtils::stringToFloat(node.val, f);
	ctUtils::insertFloat32(f, mInstrPool);	
}

void ctCodeGenerator::generate(ctProgramNode& node, std::string outfile) {

	if (mImage) {
		ct_image_free(mImage.get());
	};

	mImage = std::make_unique<ctImage>();
	mInstrPool.clear();
	mProcedures.clear();

	node.accept(*this);

	ct_image_write(mImage.get(), outfile.data());
}