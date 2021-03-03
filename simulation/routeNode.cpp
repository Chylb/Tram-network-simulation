#include "routeNode.h"

#include "passenger.h"

RouteNode::RouteNode(std::string name, std::list<int> generationDistribution, std::list<int> absorptionRate, int expectedGeneratedCount)
{
	m_name = name;
	addHistoryRow(0.0);

	std::copy(generationDistribution.begin(), generationDistribution.end(), m_generationDistribution);
	std::copy(absorptionRate.begin(), absorptionRate.end(), m_absorptionRate);

	m_expectedGeneratedCount = expectedGeneratedCount;
}

void RouteNode::addOutgoingEdge(RouteEdge* edge)
{
	m_outgoingEdges.push_back(edge);
}

void RouteNode::addIncomingEdge(RouteEdge* edge)
{
	m_incomingEdges.push_back(edge);
}

int RouteNode::randomPassengerSpawnHour(std::minstd_rand0* rng)
{
	std::uniform_int_distribution<> passenger_dist(1, m_expectedGeneratedCount);

	float passenger = passenger_dist(*rng);
	int h = 0;
	while (m_generationDistribution[h] < passenger)
		h++;

	return h;
}

void RouteNode::addPassenger(float time, Passenger* passenger, const std::list<int>& routes)
{
	m_passengerCount++;
	m_passengerQueue.enqueue(passenger, routes);
	addHistoryRow(time);
}

Passenger* RouteNode::dispensePassenger(float time, int route)
{
	Passenger* passenger = m_passengerQueue.dequeue(route);
	if (passenger != nullptr) {
		m_passengerCount--;
		addHistoryRow(time);
	}
	return passenger;
}

int RouteNode::getExpectedGeneratedCount()
{
	return m_expectedGeneratedCount;
}

int RouteNode::getAbsorptionRate(int h)
{
	return m_absorptionRate[h];
}

std::string RouteNode::getName()
{
	return m_name;
}

void RouteNode::addHistoryRow(float time)
{
	m_timeHistory.push_back(time);
	m_passengerHistory.push_back(m_passengerCount);
}

json RouteNode::getHistory()
{
	json history;
	history["name"] = m_name;
	history["time"] = m_timeHistory;
	history["passenger"] = m_passengerHistory;

	return history;
}