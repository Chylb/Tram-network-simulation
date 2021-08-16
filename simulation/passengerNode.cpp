#include "passengerNode.h"
#include "passenger.h"
#include "tools/MultilineQueue.hpp"

PassengerNode::PassengerNode(std::string name, std::list<int> generationDistribution, std::list<int> absorptionRate, int expectedGeneratedCount)
{
	m_name = name;
	addHistoryRow(0.0);

	std::copy(generationDistribution.begin(), generationDistribution.end(), m_generationDistribution);
	std::copy(absorptionRate.begin(), absorptionRate.end(), m_absorptionRate);

	m_expectedGeneratedCount = expectedGeneratedCount;
}

void PassengerNode::addOutgoingEdge(PassengerEdge* edge)
{
	m_outgoingEdges.push_back(edge);
}

void PassengerNode::addIncomingEdge(PassengerEdge* edge)
{
	m_incomingEdges.push_back(edge);
}

void PassengerNode::initializePassengerQueue()
{
	m_passengerQueue = MultilineQueue<Passenger*, PassengerEdge*>(m_outgoingEdges);
}

std::list<PassengerEdge*> PassengerNode::getOutgoingEdges() {
	return m_outgoingEdges;
}

int PassengerNode::randomPassengerSpawnHour(std::minstd_rand0* rng)
{
	std::uniform_int_distribution<> passenger_dist(1, m_expectedGeneratedCount);

	float passenger = passenger_dist(*rng);
	int h = 0;
	while (m_generationDistribution[h] < passenger)
		h++;

	return h;
}

void PassengerNode::addPassenger(float time, Passenger* passenger, const std::list<PassengerEdge*>& trip)
{
	m_passengerCount++;
	m_passengerQueue.enqueue(passenger, trip);
	addHistoryRow(time);
}

Passenger* PassengerNode::dispensePassenger(float time, PassengerEdge* edge)
{
	Passenger* passenger = m_passengerQueue.dequeue(edge);
	if (passenger != nullptr)
	{
		m_passengerCount--;
		addHistoryRow(time);
	}
	return passenger;
}

int PassengerNode::getPassengerCount(PassengerEdge* edge) {
	return m_passengerQueue.size(edge);
}

int PassengerNode::getExpectedGeneratedCount()
{
	return m_expectedGeneratedCount;
}

int PassengerNode::getAbsorptionRate(int h)
{
	return m_absorptionRate[h];
}

std::string PassengerNode::getName()
{
	return m_name;
}

void PassengerNode::addHistoryRow(float time)
{
	m_timeHistory.push_back(time);
	m_passengerHistory.push_back(m_passengerCount);
}

json PassengerNode::getHistory()
{
	json history;
	history["name"] = m_name;
	history["time"] = m_timeHistory;
	history["passenger"] = m_passengerHistory;

	return history;
}