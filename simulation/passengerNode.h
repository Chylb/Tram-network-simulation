#pragma once

#include <list>
#include <set>
#include <random>

#include "tools/MultilineQueue.hpp"

class Node;
class PassengerEdge;
class Passenger;
class Tram;

#include "include/json.hpp"
using json = nlohmann::json;

class PassengerNode
{
	friend class Graph;

public:
	PassengerNode(std::string name, std::list<int> generationDistribution, std::list<int> absorptionRate, int expectedGeneratedCount);

	void addOutgoingEdge(PassengerEdge* edge);
	void addIncomingEdge(PassengerEdge* edge);

	void initializePassengerQueue();

	std::list<PassengerEdge*> getOutgoingEdges();

	int randomPassengerSpawnHour(std::minstd_rand0* rng);

	void addPassenger(float time, Passenger* passenger, const std::list<PassengerEdge*>& trip);
	Passenger* dispensePassenger(float time, PassengerEdge* edge);
	int getPassengerCount(PassengerEdge* edge);

	int getExpectedGeneratedCount();
	int getAbsorptionRate(int h);
	std::string getName();

	json getHistory();

private:
	std::string m_name;
	std::set<Passenger*> m_passengers;
	int m_passengerCount = 0;
	MultilineQueue<Passenger*, PassengerEdge*> m_passengerQueue;

	std::list<PassengerEdge*> m_outgoingEdges;
	std::list<PassengerEdge*> m_incomingEdges;

	int m_generationDistribution[24];
	int m_absorptionRate[24];
	int m_expectedGeneratedCount;

	void addHistoryRow(float time);
	std::list<float> m_timeHistory;
	std::list<int> m_passengerHistory;
};
