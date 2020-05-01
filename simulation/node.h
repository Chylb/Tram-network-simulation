#pragma once

#include <list>

class Edge;

class Node
{
	friend class Graph;

public:
	Node(int id, bool isTrafficLight, bool isTramStop);
	Node(int id);
	void addOutgoingEdge(Edge *edge);
	void addIncomingEdge(Edge *edge);
	int getId();
	bool isTrafficLight();
	bool isTramStop();

private:
	int m_id;
	bool m_isTrafficLight;
	bool m_isTramStop;
	std::list<Edge *> m_outgoingEdges;
	std::list<Edge *> m_incomingEdges;
};