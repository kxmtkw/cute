#include "../spec/spec.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#pragma once

struct CtScope;

enum class CtNodeType
{
	RootProgram,
	Source,
	Function,

	StmtBlock,
	Declaration,
	Out,
	ExprStatement,
	If,
	Loop,
	While,
	For,

	Int,
	Float,
	Bool,
	Identifier,
	BinaryOp,
	UnaryOp,
	FunctionCall,
	Assignment,
	TypeCast
};


namespace CtNode
{
	struct Base;

	struct Object;
	struct Statement;
	struct Expression;

	// Object Nodes
	struct RootProgram;
	struct Source;
	struct Function;

	
	// Statement Nodes
	struct StmtBlock;
	struct Declaration;
	struct Out;
	struct ExprStatment;
	struct If;
	struct Loop;
	struct While;
	struct For;
	
	// Expression Nodes
	struct Int;
	struct Float;
	struct Bool;
	struct Identifier;
	struct Variable;
	struct BinaryOp;
	struct UnaryOp;
	struct FunctionCall;
	struct Assignment;
	struct TypeCast;



	struct Base
	{
		CtNodeType nt;
	};

	struct Object     : Base{};
	struct Statement  : Base{};
	struct Expression : Base{CtSpec::TypeInfo* expr_type;};


	struct RootProgram : Object
	{
		Source* src;
		CtScope* scope;
		RootProgram() {{nt = CtNodeType::RootProgram;}};
		~RootProgram();
	};

	struct Source : Object
	{
		std::map<std::string, Function*> functions;
		CtScope* scope;

		Source() {{nt = CtNodeType::Source;}};
		~Source();
	};

	struct Function : Object
	{
		std::string name;
		std::vector<Declaration*> parameters;
		StmtBlock* block;
		CtScope* scope;

		Function() {{nt = CtNodeType::Function;}};
		~Function();
	};

	
	// Statement Nodes


	struct StmtBlock : Statement
	{
		std::vector<Statement*> stmts;

		CtScope* scope;
		
		StmtBlock() {nt = CtNodeType::StmtBlock;};
		~StmtBlock();
	};


	struct Declaration : Statement
	{	
		std::string type_id;
		std::string name;
		Assignment* assignment = nullptr;
		CtSpec::TypeInfo* type;

		Declaration() {nt = CtNodeType::Declaration;};
		~Declaration();
	};


	struct Out : Statement
	{
		Expression* expr;
		Out(Expression* expr): expr(expr) {{nt = CtNodeType::Out;}};
		~Out();
	};

	struct ExprStatment : Statement
	{
		Expression* expr;

		ExprStatment(Expression* expr): expr(expr) {{nt = CtNodeType::ExprStatement;}};
		~ExprStatment();
	};

	struct If : Statement
	{
		Expression* condition;
		StmtBlock* then_block;
		Statement* else_stmt = nullptr; // can be either an else (stmtblock) or else if (If node)

		CtScope* scope;

		If() {nt = CtNodeType::If;}
		~If();
	};

	struct Loop : Statement
	{
		StmtBlock* block;

		CtScope* scope;

		Loop() {{nt = CtNodeType::Loop;}};
		~Loop();
	};


	struct While : Statement
	{
		Expression* condition;
		StmtBlock* block;

		CtScope* scope;

		While() {{nt = CtNodeType::While;}};
		~While();
	};

	struct For : Statement
	{
		Statement* init;
		Expression* condition;
		Expression* step;
		StmtBlock* block;

		CtScope* scope;

		For() {{nt = CtNodeType::For;}};
		~For();
	};
	
	
	
	// Expression Nodes
	struct Int : Expression
	{
		std::string raw;
		union
		{
			int32_t  i32;
			int64_t  i64;
			uint32_t u32;
			uint64_t u64;
		} val;

		Int(std::string i): raw(i) {nt = CtNodeType::Int;};
	};


	struct Float : Expression
	{
		std::string raw;
		union
		{
			float  f32;
			double f64;
		} val;

		Float(std::string f): raw(f) {nt = CtNodeType::Float;};
	};


	struct Bool : Expression
	{
		bool val;
		Bool(bool b): val(b) {nt = CtNodeType::Bool;}
	};


	struct Identifier : Expression
	{
		std::string val;

		Identifier(std::string val): val(val) {{nt = CtNodeType::Identifier;}};
	};

	struct BinaryOp : Expression
	{
		CtSpec::BinaryOpType op;
		Expression* left;
		Expression* right;
		BinaryOp() {nt = CtNodeType::BinaryOp;};
		BinaryOp(CtSpec::BinaryOpType op, Expression* left, Expression* right): op(op), left(left), right(right) {nt = CtNodeType::BinaryOp;};
		~BinaryOp();
	};


	struct UnaryOp : Expression
	{
		CtSpec::UnaryOpType op;
		Expression* operand;
		UnaryOp() {nt = CtNodeType::UnaryOp;};
		UnaryOp(CtSpec::UnaryOpType op, Expression* oper): op(op), operand(oper) {nt = CtNodeType::UnaryOp;};
		~UnaryOp();
	};


	struct FunctionCall : Expression
	{
		std::string name;
		std::vector<Expression*> args;

		FunctionCall() {nt = CtNodeType::FunctionCall;};
		FunctionCall(std::string name): name(name) {{nt = CtNodeType::FunctionCall;}};
		~FunctionCall();
	};

	struct Assignment : Expression
	{
		Identifier* name;
		Expression* value;

		Assignment() {nt = CtNodeType::Assignment;};
		~Assignment();
	};

	struct TypeCast : Expression
	{
		std::string to_type;
		Expression* expr;

		TypeCast() {nt = CtNodeType::TypeCast;};
		~TypeCast();
	};

};




class CtNodeWalker
{
	virtual void handleRoot(CtNode::RootProgram *node) = 0;
	virtual void handleSource(CtNode::Source *node) = 0;

	virtual void handleFunction(CtNode::Function *node) = 0;

	virtual void handleStmtBlock(CtNode::StmtBlock *node) = 0;
	virtual void handleDeclaration(CtNode::Declaration *node) = 0;
	virtual void handleIf(CtNode::If *node) = 0;
	virtual void handleLoop(CtNode::Loop *node) = 0;
	virtual void handleWhile(CtNode::While *node) = 0;
	virtual void handleFor(CtNode::For *node) = 0;
	virtual void handleOut(CtNode::Out *node) = 0;

	virtual void handleInt(CtNode::Int *node) = 0;
	virtual void handleFloat(CtNode::Float *node) = 0;
	virtual void handleBool(CtNode::Bool *node) = 0;
	virtual void handleBinaryOp(CtNode::BinaryOp *node) = 0;
	virtual void handleIdentifier(CtNode::Identifier *node) = 0;
	virtual void handleFunctionCall(CtNode::FunctionCall *node) = 0;
	virtual void handleAssignment(CtNode::Assignment *node) = 0;
	virtual void handleTypeCast(CtNode::TypeCast *node) = 0;

	public:
	
	void walk(CtNode::Base* node);
};


class CtNodePrinter: public CtNodeWalker
{
	int indent = 0;

	void printIndent();

	void handleRoot(CtNode::RootProgram *node);
	void handleSource(CtNode::Source *node);

	void handleFunction(CtNode::Function *node);

	void handleStmtBlock(CtNode::StmtBlock *node);
	void handleDeclaration(CtNode::Declaration *node);
	void handleIf(CtNode::If *node);
	void handleLoop(CtNode::Loop *node);
	void handleWhile(CtNode::While *node);
	void handleFor(CtNode::For *node);
	void handleOut(CtNode::Out *node);

	void handleInt(CtNode::Int *node);
	void handleFloat(CtNode::Float *node);
	void handleBool(CtNode::Bool *node);
	void handleBinaryOp(CtNode::BinaryOp *node);
	void handleIdentifier(CtNode::Identifier *node);
	void handleFunctionCall(CtNode::FunctionCall *node);
	void handleAssignment(CtNode::Assignment *node);
	void handleTypeCast(CtNode::TypeCast *node);

	
	public:

	void print(CtNode::Base *node);
};