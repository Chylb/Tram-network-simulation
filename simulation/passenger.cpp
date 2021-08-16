#include "passenger.h"

#include "graph.h"
#include "tram.h"
#include "PassengerNode.h"
#include "tramStop.h"

Passenger::Passenger(float time, PassengerNode* startNode, PassengerNode* endNode, std::unordered_map<PassengerNode*, std::list<PassengerEdge*>>* path)
{
	m_endNode = endNode;
	m_path = path;
	m_currentNode = startNode;
	m_currentTram = nullptr;
	startNode->addPassenger(time, this, (*m_path)[m_currentNode]);
	//addHistoryRow(time);
}

void Passenger::notifyInside(float time, TramStop* tramStop)
{
	for (auto edge : (*m_path)[tramStop->m_passengerNode])
	{
		if (edge == m_currentTram->getPassengerEdge())
			return;
	}

	m_currentTram->requestPassengerExit(this);
}

void Passenger::enterTram(float time, Tram* tram)
{
	m_currentTram = tram;
	m_currentNode = nullptr;

	//addHistoryRow(time);
}

void Passenger::exitTram(float time, PassengerNode* node)
{
	m_currentTram = nullptr;
	m_currentNode = node;

	if (node == m_endNode)
	{
		//delete this;
	}
	else
	{
		node->addPassenger(time, this, (*m_path)[m_currentNode]);
	}
	//addHistoryRow(time);
}

void Passenger::addHistoryRow(float time)
{
	m_timeHistory.push_back(time);
	m_tramHistory.push_back(m_currentTram);
	m_nodeHistory.push_back(m_currentNode);
}