#include "node.h"

Node::Node(unsigned long int id) {
	m_id = id;
}
void Node::addOutgoingEdge(Edge* edge)
{
	m_outgoingEdges.emplace_back(edge);
}

void Node::addIncomingEdge(Edge* edge)
{
	m_incomingEdges.emplace_back(edge);
}

unsigned long int Node::getId() {
	return m_id;
}