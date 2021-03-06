#pragma once

#include "Symbol.h"

class Message
{
private:
	Symbol symbol;
	int action;
	int sender;
	bool isSelection;

public:
	Message(Symbol s, int action, int sender, bool isSelection = false);
	//Message(std::vector<int> position, int action, int sender, bool isSelection = false);
	~Message();
	Symbol getSymbol() const;
	int getAction() const;
	int getSenderId();
	bool getIsSelection();
};

