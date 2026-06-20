#ifndef PARSER_NODES_HPP
#define PARSER_NODES_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct ctNode;

struct ctProgramNode;
struct ctProcedureNode;
struct ctOperationNode;

struct ctOperandNode;

struct ctWordNode;
struct ctRegisterNode;
struct ctSlotNode;
struct ctLiteralNode;

struct ctIntNode;
struct ctFloatNode;

class ctNodeVisitor {
public:

    virtual ~ctNodeVisitor() = default;
    virtual void visit(ctProgramNode& node) = 0;
    virtual void visit(ctProcedureNode& node) = 0;
    virtual void visit(ctOperationNode& node) = 0;
	virtual void visit(ctWordNode& node) = 0;
	virtual void visit(ctRegisterNode& node) = 0;
    virtual void visit(ctSlotNode& node) = 0;
	virtual void visit(ctIntNode& node) = 0;
	virtual void visit(ctFloatNode& node) = 0;
};

struct ctNode {
public:

    virtual ~ctNode() = default;
    virtual void accept(ctNodeVisitor& visitor) = 0;

};


struct ctProgramNode: public ctNode {
	std::vector<std::unique_ptr<ctProcedureNode>> procedures;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};

struct ctProcedureNode : public ctNode {
	uint id;
	std::vector<std::unique_ptr<ctOperationNode>> operations;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};


struct ctOperationNode : public ctNode {
	std::string opcode;
	std::vector<std::unique_ptr<ctOperandNode>> operands;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};


struct ctOperandNode : public ctNode {};

struct ctRegisterNode : public ctOperandNode {
	std::string val;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};


struct ctSlotNode : public ctOperandNode {
	std::string val;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};


struct ctWordNode : public ctOperandNode {
	std::string val;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};


struct ctLiteralNode : public ctOperandNode {};

struct ctIntNode : public ctLiteralNode {
	std::string val;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};

struct ctFloatNode : public ctLiteralNode {
	std::string val;
	void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
};




class ctNodePrinter : public ctNodeVisitor {
public:
	void visit(ctProgramNode& node) override {
		std::cout << "Program:\n";
		for (auto& proc : node.procedures) {
			proc->accept(*this);
		}
	}

	void visit(ctProcedureNode& node) override {
		std::cout << "  Procedure [" << node.id << "]:\n";
		for (auto& op : node.operations) {
			op->accept(*this);
		}
	}

	void visit(ctOperationNode& node) override {
		std::cout << "    Operation [" << node.opcode << "]:\n";
		for (auto& operand : node.operands) {
			std::cout << "      ";
			operand->accept(*this);
			std::cout << "\n";
		}
	}

	void visit(ctRegisterNode& node) override {
		std::cout << "Reg: " << node.val;
	}

	void visit(ctSlotNode& node) override {
		std::cout << "Slot: " << node.val;
	}

	void visit(ctWordNode& node) override {
		std::cout << "Word: " << node.val;
	}

	void visit(ctIntNode& node) override {
		std::cout << "Int: " << node.val;
	}

	void visit(ctFloatNode& node) override {
		std::cout << "Float: " << node.val;
	}
};


#endif // PARSER_NODES_HPP