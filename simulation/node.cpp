#include "node.h"

Node::Node(int id, bool isTrafficLight, bool isTramStop)
{
	m_id = id;
	m_isTrafficLight = isTrafficLight;
	m_isTramStop = isTramStop;
}

Node::Node(int id)
{
	m_id = id;
	m_isTrafficLight = false;
	m_isTramStop = false;
}

void Node::addOutgoingEdge(Edge *edge)
{
	m_outgoingEdges.emplace_back(edge);
}

void Node::addIncomingEdge(Edge *edge)
{
	m_incomingEdges.emplace_back(edge);
}

bool Node::isTrafficLight()
{
	return m_isTrafficLight;
}

bool Node::isTramStop()
{
	return m_isTramStop;
}

int Node::getId()
{
	return m_id;
}