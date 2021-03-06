#pragma once

#include <list>
#include <set>
#include <random>

#include "MultilineQueue.hpp"

class Node;
class RouteEdge;
class Passenger;
class Tram;

#include "include/json.hpp"
using json = nlohmann::json;

class RouteNode
{
    friend class Graph;

public:
    RouteNode(std::string name, std::list<int> generationDistribution, std::list<int> absorptionRate, int expectedGeneratedCount);

    void addOutgoingEdge(RouteEdge *edge);
    void addIncomingEdge(RouteEdge *edge);

    void initializePassengerQueue();

    std::list<RouteEdge *> getOutgoingEdges();

    int randomPassengerSpawnHour(std::minstd_rand0 *rng);

    void addPassenger(float time, Passenger *passenger, const std::list<RouteEdge *> &routes);
    Passenger *dispensePassenger(float time, RouteEdge *route);
    int getPassengerCount(RouteEdge *route);

    int getExpectedGeneratedCount();
    int getAbsorptionRate(int h);
    std::string getName();

    json getHistory();

private:
    std::string m_name;
    std::set<Passenger *> m_passengers;
    int m_passengerCount = 0;
    MultilineQueue<Passenger *, RouteEdge *> m_passengerQueue;

    std::list<RouteEdge *> m_outgoingEdges;
    std::list<RouteEdge *> m_incomingEdges;

    int m_generationDistribution[24];
    int m_absorptionRate[24];
    int m_expectedGeneratedCount;

    void addHistoryRow(float time);
    std::list<float> m_timeHistory;
    std::list<int> m_passengerHistory;
};
