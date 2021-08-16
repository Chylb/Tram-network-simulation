#include "edge.h"

#include "tram.h"

Edge::Edge(int id, Node* tail, Node* head, float length, float maxspeed)
{
	m_id = id;
	m_tail = tail;
	m_head = head;
	m_length = length;
	m_maxspeed = maxspeed;
}

Node* Edge::getHead()
{
	return m_head;
}

Node* Edge::getTail()
{
	return m_tail;
}

void Edge::addTram(Tram* tram)
{
	if (m_trams.size() != 0)
		m_nextTram[tram] = m_trams.front();
	else
		m_nextTram[tram] = nullptr;
	m_trams.push_front(tram);
}

void Edge::removeTram()
{
	auto removed = m_trams.back();
	m_trams.pop_back();

	if (m_trams.size() != 0) {
		Tram* lastTram = m_trams.back();
		m_nextTram[lastTram] = nullptr;
	}

	m_nextTram.erase(removed);
}

float Edge::getLength()
{
	return m_length;
}

std::list<Tram*> Edge::getTrams()
{
	return m_trams;
}

Tram* Edge::getTramAhead(Tram* tram)
{
	return m_nextTram[tram];
}

int Edge::getId() {
	return m_id;
}
