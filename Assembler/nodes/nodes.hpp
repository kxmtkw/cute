#ifndef PARSER_NODES_HPP
#define PARSER_NODES_HPP

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "symbols/symbols.hpp"

struct ctNode;

struct ctProgramNode;
struct ctProcedureNode;
struct ctStationNode;
struct ctOperationNode;

struct ctOperandNode;

struct ctWordNode;
struct ctSlotNode;
struct ctLiteralNode;

struct ctIntNode;
struct ctFloatNode;

enum class NodeType {
    Program,
    Procedure,
	Station,
    Operation,
    Word,
    Slot,
    Int,
    Float
};


class ctNodeVisitor {
public:

    virtual ~ctNodeVisitor() = default;
    virtual void visit(ctProgramNode& node) = 0;
    virtual void visit(ctProcedureNode& node) = 0;
    virtual void visit(ctOperationNode& node) = 0;

	virtual void visit(ctWordNode& node) = 0;
    virtual void visit(ctSlotNode& node) = 0;

	virtual void visit(ctIntNode& node) = 0;
	virtual void visit(ctFloatNode& node) = 0;
};



struct ctNode {
    virtual ~ctNode() = default;
    virtual void accept(ctNodeVisitor& visitor) = 0;
    virtual NodeType getType() const = 0;
};

struct ctProgramNode : public ctNode {
    std::vector<std::unique_ptr<ctProcedureNode>> procedures;
	std::map<std::string, ctSymbol> symbolmap;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Program; }
};

struct ctProcedureNode : public ctNode {
    std::string name;
    std::vector<std::unique_ptr<ctNode>> operations;
	unsigned int assigned_id;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Procedure; }
};

struct ctOperationNode : public ctNode {
    std::string opcode;
    std::vector<std::unique_ptr<ctOperandNode>> operands;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Operation; }
};

struct ctOperandNode : public ctNode {
    virtual void accept(ctNodeVisitor& visitor) override = 0;
};


struct ctSlotNode : public ctOperandNode {
    std::string val;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Slot; }
};

struct ctWordNode : public ctOperandNode {
    std::string val;

	ctSymbol* sym = nullptr;
    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Word; }
};

struct ctLiteralNode : public ctOperandNode {
    virtual void accept(ctNodeVisitor& visitor) override = 0;
};

struct ctIntNode : public ctLiteralNode {
    std::string val;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Int; }
};

struct ctFloatNode : public ctLiteralNode {
    std::string val;

    void accept(ctNodeVisitor& visitor) override { visitor.visit(*this); }
    NodeType getType() const override { return NodeType::Float; }
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
		std::cout << "  Procedure [" << node.name << "]:\n";
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