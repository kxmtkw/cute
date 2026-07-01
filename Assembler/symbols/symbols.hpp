#ifndef SYMBOLS_SYMBOLS_HPP
#define SYMBOLS_SYMBOLS_HPP

#include <string>

struct ctNode;

enum class ctSymbolType {
	Procedure,
};

struct ctSymbol {
	ctSymbolType type;
	std::string name;
	const ctNode* node;

	ctSymbol(ctSymbolType _type, std::string _name, const ctNode* _node):
	type(_type), name(_name), node(_node) {}
};

#endif // SYMBOLS_SYMBOLS_HPP