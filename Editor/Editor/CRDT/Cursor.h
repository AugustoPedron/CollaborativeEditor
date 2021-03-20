#pragma once

#include <vector>

class Cursor
{
private:
	std::vector<int> m_cursorPosition;
	int m_senderId;

public:
	Cursor(int senderId, std::vector<int> m_cursorPosition);
};

