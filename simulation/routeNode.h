#pragma once

#include <list>
#include <random>

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

    int randomPassengerSpawnHour(std::minstd_rand0 *rng);

    void addPassenger(float time, Passenger *passenger);
    void removePassenger(float time, Passenger *passenger);
    void notifyPassengers(float time, Tram *tram);

    int getExpectedGeneratedCount();
    int getAbsorptionRate(int h);
    std::string getName();

    json getHistory();

private:
    std::string m_name;
    std::list<Passenger *> m_passengers;

    std::list<RouteEdge *> m_outgoingEdges;
    std::list<RouteEdge *> m_incomingEdges;

    int m_generationDistribution[24];
    int m_absorptionRate[24];
    int m_expectedGeneratedCount;

    void addHistoryRow(float time);
    std::list<float> m_timeHistory;
    std::list<int> m_passengerHistory;
};
