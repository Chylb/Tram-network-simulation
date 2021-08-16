#include "passengerEdge.h"

class PassengerNode;

PassengerEdge::PassengerEdge(std::list<int> lines, PassengerNode* tail, PassengerNode* head) {
	m_lines = lines;
	m_tail = tail;
	m_head = head;
}

PassengerNode* PassengerEdge::getHead() {
	return m_head;
}