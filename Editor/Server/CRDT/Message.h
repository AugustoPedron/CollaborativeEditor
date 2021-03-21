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
	Message(Symbol s, int action,int sender);
	~Message();
	Symbol getSymbol() const;
	int getAction() const;
	int getSenderId();
    bool getIsSelection();
};

