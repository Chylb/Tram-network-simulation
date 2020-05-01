#include "edge.h"

#include "tram.h"

Edge::Edge(int id, Node *tail, Node *head, float length, float maxspeed)
{
	m_id = id;
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
	m_nextTram[tram] = m_trams.front();
	m_trams.push_front(tram);
}

void Edge::removeTram()
{
	auto removed = m_trams.back();
	m_trams.pop_back();

	Tram *lastTram = m_trams.back();
	if (lastTram != nullptr)
		m_nextTram[lastTram] = nullptr;

	m_nextTram.erase(removed);
}

float Edge::getLength()
{
	return m_length;
}

std::list<float> Edge::getTramsDistances()
{
	return m_tramsDistances;
}

std::list<Tram *> Edge::getTrams()
{
	return m_trams;
}

Tram *Edge::getTramAhead(Tram *tram)
{
	return m_nextTram[tram];
}

int Edge::getId(){
	return m_id;
}
