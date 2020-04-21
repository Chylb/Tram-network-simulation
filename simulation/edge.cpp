#include "edge.h"

#include "tram.h"

Edge::Edge(Node *tail, Node *head, float length, float maxspeed)
{
	m_tail = tail;
	m_head = head;
	m_length = length;
	m_maxspeed = maxspeed;
}

Node *Edge::getHead()
{
	return m_head;
}

Node *Edge::getTail()
{
	return m_tail;
}

void Edge::addTram(Tram *tram)
{
	m_trams.push_front(tram);
}