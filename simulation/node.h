#pragma once

#include <list>

class Edge;

class Node
{
	friend class Graph;

private:
	unsigned long int m_id;
	std::list<Edge *> m_outgoingEdges;
	std::list<Edge *> m_incomingEdges;
	//nlohmann::json& m_tags;
public:
	Node(unsigned long int id);
	void addOutgoingEdge(Edge *edge);
	void addIncomingEdge(Edge *edge);
	unsigned long int getId();
};