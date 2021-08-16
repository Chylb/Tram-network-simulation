#pragma once
#include <list>
#include <map>

class Node;
class Tram;
class PassengerNode;

class PassengerEdge
{
	friend class Graph;

public:
	PassengerEdge(std::list<int> lines, PassengerNode* tail, PassengerNode* head);
	PassengerNode* getHead();

private:
	std::list<int> m_lines;
	PassengerNode* m_head;
	PassengerNode* m_tail;
};
