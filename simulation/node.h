#pragma once

#include <list>

class Edge;

class Node
{
	friend class Graph;

public:
	Node(int id, bool isTrafficLight, bool isJunctionExit, bool isTramStop);
	Node(int id);
	void addOutgoingEdge(Edge* edge);
	void addIncomingEdge(Edge* edge);
	int getId();
	bool isTrafficLight();
	bool isJunctionExit();
	bool isTramStop();
	std::list<Edge*> getIncomingEdges();

protected:
	int m_id;
	bool m_isTrafficLight;
	bool m_isTramStop;
	bool m_isJunctionExit;
	std::list<Edge*> m_outgoingEdges;
	std::list<Edge*> m_incomingEdges;
};