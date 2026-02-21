#include <format>

#include "../node/node.hpp"

#include "../node/scope.hpp"
#include "../spec/error.hpp"
#include "name_resolver.hpp"



void CtNameResolver::handleRoot(CtNode::RootProgram *node)
{
	this->walk(node->src);
}


void CtNameResolver::handleSource(CtNode::Source *node)
{
	for (auto func: node->functions)
	{
		this->walk(func.second);
	}
}

void CtNameResolver::handleFunction(CtNode::Function *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtNameResolver::handleStmtBlock(CtNode::StmtBlock *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	for (auto stmt: node->stmts)
	{
		this->walk(stmt);
	}

	this->current_scope = node->scope->parent;
}


void CtNameResolver::handleDeclaration(CtNode::Declaration *node)
{	
	CtScope::Defintion def;
	if (this->current_scope->has(node->name, def))
	{
		CtError::raise(
			CtError::ErrorType::NameError, 
			std::format("Variable already defined within scope: {}", node->name)
		);
	};

	this->current_scope->definitions[node->name] = CtScope::Defintion(CtScope::DefKind::Variable, node->name, node->type_id);

	if (node->assignment) {this->walk(node->assignment);}
}


void CtNameResolver::handleOut(CtNode::Out *node)
{
	this->walk(node->expr);
}


void CtNameResolver::handleLoop(CtNode::Loop *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtNameResolver::handleWhile(CtNode::While *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	this->walk(node->condition);
	this->walk(node->block);

	this->current_scope = node->scope->parent;
}

void CtNameResolver::handleFor(CtNode::For *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	this->walk(node->init);
	this->walk(node->condition);
	this->walk(node->step);
	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtNameResolver::handleIf(CtNode::If *node)
{
	node->scope = new CtScope(this->current_scope);
	this->current_scope = node->scope;

	this->walk(node->condition);
	this->walk(node->then_block);

	this->current_scope = node->scope->parent;

	if (node->else_stmt) {this->walk(node->else_stmt);}
}


void CtNameResolver::handleAssignment(CtNode::Assignment *node)
{
	CtScope::Defintion def;
	if (!this->current_scope->search(node->name->val, def))
	{
		CtError::raise(
			CtError::ErrorType::NameError, 
			std::format("Undefined variable: {}", node->name->val)
		);
	};
	this->walk(node->value);
}


void CtNameResolver::handleInt(CtNode::Int *node)
{
	// does not need to do anything
};

void CtNameResolver::handleFloat(CtNode::Float *node)
{
	// does not need to do anythings
};

void CtNameResolver::handleBool(CtNode::Bool *node)
{

};

void CtNameResolver::handleBinaryOp(CtNode::BinaryOp *node)
{
	this->walk(node->left);
	this->walk(node->right);
};

void CtNameResolver::handleIdentifier(CtNode::Identifier *node)
{
	CtScope::Defintion def;

	if (!this->current_scope->search(node->val, def))
	{
		CtError::raise(
			CtError::ErrorType::NameError, 
			std::format("Undefined variable: {}", node->val)
		);
	};
}

void CtNameResolver::handleFunctionCall(CtNode::FunctionCall *node)
{
	// nothing implemented
}


void CtNameResolver::handleTypeCast(CtNode::TypeCast *node)
{
	this->walk(node->expr);
}


CtNode::RootProgram* CtNameResolver::analyze(CtNode::RootProgram* root)
{
	this->root = root;
	this->current_scope = nullptr;
	this->walk(root);
	return root;
}

