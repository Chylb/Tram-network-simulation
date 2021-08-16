#pragma once
#include <list>
#include <map>

class Node;
class Tram;

class Edge
{
	friend class Graph;

public:
	Edge(int id, Node* tail, Node* head, float length, float maxspeed);
	Node* getHead();
	Node* getTail();

	void addTram(Tram* tram);
	void removeTram();

	int getId();
	float getLength();
	std::list<Tram*> getTrams();
	Tram* getTramAhead(Tram* tram);

private:
	int m_id;
	Node* m_head;
	Node* m_tail;
	float m_length;
	float m_maxspeed;
	std::list<Tram*> m_trams;
	std::map<Tram*, Tram*> m_nextTram;
};
