#include "parser.hpp"
#include "tokenizer/tokens.hpp"
#include <string>

std::unique_ptr<ctProgramNode> ctParser::parse(ctTokenStream& stream) {
	mStream = &stream;
	auto program = std::make_unique<ctProgramNode>();

	while (mStream->peek().type != ctTokenType::EndOfFile) {
		auto proc = parseProcedure();
		if (proc) {
			program->procedures.push_back(std::move(proc));
		}
	}

	return program;
}

std::unique_ptr<ctProcedureNode> ctParser::parseProcedure() {

	std::string word;
	if (!mStream->expectTokenType(ctTokenType::Word, word) || word != "proc") {
		return nullptr;
	}


	std::string id_str;
	if (!mStream->expectTokenType(ctTokenType::Word, id_str)) {
		return nullptr;
	}

	auto proc = std::make_unique<ctProcedureNode>();
	proc->name = id_str;

	if (!mStream->expectToken("[")) {
		return nullptr;
	}

	while (mStream->peek().type != ctTokenType::EndOfFile && mStream->peek().type != ctTokenType::Symbol) {
		ctToken token = mStream->peek();
		if (token.type == ctTokenType::Symbol && mStream->getValue(token) == "]") {
			break;
		}
		
		auto op = parseOperation();
		if (op) {
			proc->operations.push_back(std::move(op));
		}
	}

	if (!mStream->expectToken("]")) {
		return nullptr;
	}

	return proc;
}

std::unique_ptr<ctOperationNode> ctParser::parseOperation() {

	std::string opcode;
	if (!mStream->expectTokenType(ctTokenType::Word, opcode)) {
		return nullptr;
	}

	auto op = std::make_unique<ctOperationNode>();
	op->opcode = opcode;

	while (mStream->peek().type != ctTokenType::EndOfFile) {

		ctToken token = mStream->peek();

		if (mStream->expectToken(";")) {
			break;
		}

		std::string sign = "";
		if (mStream->expectToken("-")) {
			sign = "-";
			token = mStream->peek();
		}

		std::unique_ptr<ctOperandNode> operand;
		if (token.type == ctTokenType::Int) {
			auto node = std::make_unique<ctIntNode>();
			std::string val = mStream->getValue(token);
			node->val = sign + val;
			operand = std::move(node);
			mStream->next();

		} else if (token.type == ctTokenType::Float) {
			auto node = std::make_unique<ctFloatNode>();
			std::string val = mStream->getValue(token);
			node->val = sign + val;
			operand = std::move(node);
			mStream->next();

		} else if (token.type == ctTokenType::Word) {
			std::string val = mStream->getValue(token);

			if (val.empty() == false && val[0] == 'r') {
				auto node = std::make_unique<ctRegisterNode>();
				node->val = val;
				operand = std::move(node);

			} else if (val.empty() == false && val[0] == 's') {
				auto node = std::make_unique<ctSlotNode>();
				node->val = val;
				operand = std::move(node);

			} else {
				auto node = std::make_unique<ctWordNode>();
				node->val = val;
				operand = std::move(node);

			}
			mStream->next();
		} else {
			mStream->next();
		}

		if (operand) {
			op->operands.push_back(std::move(operand));
		}
	}

	if (!mStream->expectToken(";")) {
		// In some cases the operation might not end with ; if it's the end of the proc
		// But schema says ; denotes end of operation.
	}

	return op;
}
