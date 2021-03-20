#include "Message.h"



Message::Message(Symbol s, int action, int sender, bool isSelection ) :symbol(s), action(action), sender(sender), isSelection(isSelection)
{
}



//Message::Message(std::vector<int> position, int action, int sender, bool isSelection) :cursor_position(position), action(action), sender(sender), isSelection(isSelection)
//{
//	this->symbol = Symbol();
//}


Message::~Message()
{
}

Symbol Message::getSymbol() const
{
	return symbol;
}

int Message::getAction() const
{
	return action;
}

int Message::getSenderId()
{
	return this->sender;
}

bool Message::getIsSelection() {
	return this->isSelection;
}
