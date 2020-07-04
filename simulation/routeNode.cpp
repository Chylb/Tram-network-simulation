#include "routeNode.h"

#include "passenger.h"

RouteNode::RouteNode(std::string name)
{
    m_name = name;
    addHistoryRow(0.0);
}

void RouteNode::addOutgoingEdge(RouteEdge *edge)
{
    m_outgoingEdges.push_back(edge);
}

void RouteNode::addIncomingEdge(RouteEdge *edge)
{
    m_incomingEdges.push_back(edge);
}

void RouteNode::addPassenger(float time, Passenger *passenger)
{
    m_passengers.push_back(passenger);
    addHistoryRow(time);
}

void RouteNode::removePassenger(float time, Passenger *passenger)
{
    m_passengers.remove(passenger);
    addHistoryRow(time);
}

void RouteNode::notifyPassengers(float time, Tram *tram)
{
    for (auto passenger : m_passengers)
    {
        passenger->notifyOutside(time, tram);
    }
}

void RouteNode::addHistoryRow(float time)
{
    m_timeHistory.push_back(time);
    m_passengerHistory.push_back(m_passengers.size());
}

json RouteNode::getHistory()
{
    json history;
    history["name"] = m_name;
    history["time"] = m_timeHistory;
    history["passenger"] = m_passengerHistory;

    return history;
}