#pragma once
#include <vector>
#include <exception>
#include <fstream>
#include <string>
#include <iostream>

#include "Symbol.h"
#include "Message.h"
#include "Cursor.h"
#include "../Editor/UserInterval.h"

#define INSERT 0
#define DELETE_S 1
#define CHANGE 2

class CRDT
{
private:

	int m_senderId;
	int m_counter;
	std::vector<Symbol> m_symbols;
	std::vector<UserInterval> m_usersInterval;
	std::map<int, std::vector<Symbol>::iterator> m_remoteUserCursorPos;
	std::vector<Symbol>::iterator m_localCursorPos;
	//uso __int64 per evitare warning e perdita dati per troncamento
	__int64 insert_symbol(Symbol symbol);
	__int64 delete_symbol(Symbol symbol);
	__int64 change_symbol(Symbol symbol);
	QString crdt_serialize();

public:
	CRDT(int id);
	~CRDT();

	Message localInsert(int index, char value, QFont font, QColor color, Qt::AlignmentFlag alignment);
	Message localErase(int index);
	Message localChange(int index, char value, QFont font, const QColor color, Qt::AlignmentFlag alignment);

	int getId();
	Symbol getSymbol(int index);
	__int64 process(const Message& m);
	std::string to_string();//usare Qstring??
	std::vector<Message> getMessageArray();//SERVER ONLY-->questo vettore va mandato con un for ai socket con all'interno un serializzatore mando i messaggi uno alla volta
	std::vector<Message> readFromFile(std::string fileName);
	void saveOnFile(std::string filename);//versione base salva solo i caratteri e non il formato--> da testare
	bool isEmpty();
	void updateUserInterval();
	inline std::vector<UserInterval>* getUsersInterval() { return &m_usersInterval; };
	void setSiteCounter(int siteCounter);
	inline int getSiteCounter() { return m_counter; };
	Cursor getCursorPosition(int index);
	std::vector<Symbol>::iterator getCursorPosition(std::vector<int> crdtPos);
	__int64 convertIteratorToIntPos(std::vector<Symbol>::iterator it);
	void addRemoteUser(int userId, std::vector<int> pos);
	void updateRemoteUserPos(int userId, std::vector<int> pos);
	void updateLocalUserPos(int index);

	//for fractional position debug only
	void printPositions()
	{
		for (Symbol s : m_symbols) {

			std::vector<int> dv = s.getPos();
			for (int d : dv)
				std::cout << d << " ";

			std::cout << std::endl;

		}
		std::cout << std::endl;
	}

};

