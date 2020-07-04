#pragma once
#include <list>

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
    RouteNode(std::string name);

    void addOutgoingEdge(RouteEdge *edge);
    void addIncomingEdge(RouteEdge *edge);

    void addPassenger(float time, Passenger *passenger);
    void removePassenger(float time, Passenger *passenger);
    void notifyPassengers(float time, Tram *tram);

    json getHistory();

private:
    std::string m_name;
    std::list<Passenger *> m_passengers;

    std::list<RouteEdge *> m_outgoingEdges;
    std::list<RouteEdge *> m_incomingEdges;

    void addHistoryRow(float time);
    std::list<float> m_timeHistory;
    std::list<int> m_passengerHistory;
};
