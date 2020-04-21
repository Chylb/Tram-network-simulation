#pragma once
#include <list>

class Node;
class Tram;

class Edge
{
	friend class Graph;

public:
	Edge(Node *tail, Node *head, float length, float maxspeed);
	Node *getHead();
	Node *getTail();
	void addTram(Tram* tram);

private:
	Node *m_head;
	Node *m_tail;
	float m_length;
	float m_maxspeed;
	std::list<float> m_tramDistances;
	std::list<Tram*> m_trams;
};
