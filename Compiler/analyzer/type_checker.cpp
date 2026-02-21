#include "../node/node.hpp"
#include "../node/scope.hpp"
#include "../spec/error.hpp"

#include <format>


#include "type_checker.hpp"



void CtTypeChecker::handleRoot(CtNode::RootProgram *node)
{
	this->walk(node->src);
}


void CtTypeChecker::handleSource(CtNode::Source *node)
{
	for (auto func: node->functions)
	{
		this->walk(func.second);
	}
}


void CtTypeChecker::handleFunction(CtNode::Function *node)
{
	this->current_scope = node->scope;
	this->walk(node->block);
	this->current_scope = node->scope->parent;
}


void CtTypeChecker::handleStmtBlock(CtNode::StmtBlock *node)
{
	this->current_scope = node->scope;
	for (auto stmt: node->stmts)
	{
		this->walk(stmt);
	}
	this->current_scope = node->scope->parent;
}


void CtTypeChecker::handleDeclaration(CtNode::Declaration *node)
{	
	// assuming the name resolver did its job
	auto& var = this->current_scope->definitions.at(node->name);
	
	if (!primitiveTypes.contains(var.type_id))
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("Unknown type: {}", var.type_id)
		);
	}

	var.type_info = primitiveTypes.at(var.type_id);

	if (node->assignment) {this->walk(node->assignment);}
}


void CtTypeChecker::handleOut(CtNode::Out *node)
{
	this->walk(node->expr);
}


void CtTypeChecker::handleLoop(CtNode::Loop *node)
{
	this->current_scope = node->scope;

	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtTypeChecker::handleWhile(CtNode::While *node)
{
	this->current_scope = node->scope;

	this->walk(node->condition);
	if (*node->condition->expr_type != *primitiveTypes.at("bool"))
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("While loop condition must evaluate to type 'bool', not {}.", node->condition->expr_type->name)
		);
	}

	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtTypeChecker::handleFor(CtNode::For *node)
{
	this->current_scope = node->scope;

	this->walk(node->init);

	this->walk(node->condition);
	if (*node->condition->expr_type != *primitiveTypes.at("bool"))
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("For loop condition must evaluate to type 'bool', not {}.", node->condition->expr_type->name)
		);
	}

	this->walk(node->step);
	this->walk(node->block);

	this->current_scope = node->scope->parent;
}


void CtTypeChecker::handleIf(CtNode::If *node)
{
	this->current_scope = node->scope;

	this->walk(node->condition);
	if (*node->condition->expr_type != *primitiveTypes.at("bool"))
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("If condition must evaluate to type 'bool', not {}.", node->condition->expr_type->name)
		);
	}

	this->walk(node->then_block);

	this->current_scope = node->scope->parent;
	
	if (node->else_stmt) {this->walk(node->else_stmt);}
}


void CtTypeChecker::handleAssignment(CtNode::Assignment *node)
{
	CtScope::Defintion def;

	if (!this->current_scope->search(node->name->val, def))
	{
		CtError::raise(
			CtError::ErrorType::CompilerError, 
			std::format("Undefined variable: {}. Should have been resolved by name resolver. Caught in type checker.", node->name->val)
		);
	};

	this->walk(node->value);

	if (*def.type_info != *node->value->expr_type)
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("Expression of type {} can not be assigned to variable of type {}", node->value->expr_type->name, def.type_info->name)
		);
	}

	node->expr_type = node->value->expr_type;
}


void CtTypeChecker::handleInt(CtNode::Int *node)
{
	if (CtSpec::strToInt(node->raw, node->val.i64))
	{
		node->expr_type = primitiveTypes.at("int");
	}
	else if (CtSpec::strToUInt(node->raw, node->val.u64))
	{
		node->expr_type = primitiveTypes.at("uint");
	}
	else 
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("Invalid integar : {}", node->raw)
		);
	}
}


void CtTypeChecker::handleFloat(CtNode::Float *node)
{
	if (CtSpec::strToFloat(node->raw, node->val.f64))
	{
		node->expr_type = primitiveTypes.at("float");
	}
	else 
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("Invalid floating number : {}", node->raw)
		);
	}
}


void CtTypeChecker::handleBool(CtNode::Bool *node)
{
	node->expr_type = primitiveTypes.at("bool");
};


void CtTypeChecker::handleBinaryOp(CtNode::BinaryOp *node)
{
	this->walk(node->left);
	this->walk(node->right);

	if (CtSpec::isArithmetic(node->op))
	{

		if (node->left->expr_type != node->right->expr_type)
		{
			CtError::raise(
				CtError::ErrorType::TypeError, 
				std::format(
					"Binary operation '{}' not supported for types {} and {}.", 
					int(node->op), node->left->expr_type->name, node->right->expr_type->name
				)
			);
		}

		node->expr_type = node->left->expr_type;
		return;
	}

	
	if (CtSpec::isComparison(node->op))
	{
		if (node->left->expr_type != node->right->expr_type)
		{
			CtError::raise(
				CtError::ErrorType::TypeError, 
				std::format(
					"Binary operation '{}' not supported for types {} and {}.", 
					int(node->op), node->left->expr_type->name, node->right->expr_type->name
				)
			);
		}

		node->expr_type = primitiveTypes.at("bool");
		return;
	}


	if (CtSpec::isLogical(node->op))
	{
		if (node->left->expr_type != node->right->expr_type && node->left->expr_type != primitiveTypes.at("bool"))
		{
			CtError::raise(
				CtError::ErrorType::TypeError, 
				std::format(
					"Logical operation '{}' not supported for types {} and {}. Both must be booleans.", 
					int(node->op), node->left->expr_type->name, node->right->expr_type->name
				)
			);
		}

		node->expr_type = primitiveTypes.at("bool");
		return;
	}


	if (CtSpec::isBitwise(node->op))
	{
		auto int_type = primitiveTypes.at("int");
		// uint not added for simplicity, for now.
		if (node->right->expr_type != int_type || node->right->expr_type != node->left->expr_type)
		{
			CtError::raise(
				CtError::ErrorType::TypeError, 
				std::format(
					"Bitwise operation '{}' not supported for types {} and {}.", 
					int(node->op), node->left->expr_type->name, node->right->expr_type->name
				)
			);
		}

		node->expr_type = node->left->expr_type;
	}
	
}


void CtTypeChecker::handleIdentifier(CtNode::Identifier *node)
{
	CtScope::Defintion def;

	if (!this->current_scope->search(node->val, def))
	{
		CtError::raise(
			CtError::ErrorType::CompilerError, 
			std::format("Undefined variable: {}. Should have been resolved by name resolver. Caught in type checker.", node->val)
		);
	};

	node->expr_type = def.type_info;
}


void CtTypeChecker::handleFunctionCall(CtNode::FunctionCall *node)
{
	// Not Implemented
}


void CtTypeChecker::handleTypeCast(CtNode::TypeCast *node)
{
	if (!primitiveTypes.contains(node->to_type))
	{
		CtError::raise(
			CtError::ErrorType::TypeError, 
			std::format("Unknown type specified for type casting: {}", node->to_type)
		);
	}
	node->expr_type = primitiveTypes.at(node->to_type);
	this->walk(node->expr);
}


CtNode::RootProgram* CtTypeChecker::analyze(CtNode::RootProgram* root)
{
	this->root = root;
	this->current_scope = nullptr;
	this->walk(root);
	return root;
}