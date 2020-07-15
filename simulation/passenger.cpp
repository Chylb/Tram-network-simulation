#include "passenger.h"

#include "graph.h"
#include "tram.h"
#include "routeNode.h"
#include "tramStop.h"

Passenger::Passenger(float time, RouteNode *startNode, RouteNode *endNode, std::unordered_map<RouteNode *, std::list<int>> *path)
{
    m_endNode = endNode;
    m_path = path;
    m_currentNode = startNode;
    m_currentTram = nullptr;
    startNode->addPassenger(time, this);

    //addHistoryRow(time);
}

void Passenger::notifyOutside(float time, Tram *tram)
{
    for (int route : (*m_path)[m_currentNode])
    {
        if (route == tram->getRoute())
        {
            tram->requestPassengerEntrance(this);
            m_entranceRequestsTrams.push_back(tram);
            return;
        }
    }
}

void Passenger::notifyInside(float time, TramStop *tramStop)
{
    for (int route : (*m_path)[tramStop->m_routeNode])
    {
        if (route == m_currentTram->getRoute())
            return;
    }

    m_currentTram->requestPassengerExit(this);
}

void Passenger::enterTram(float time, Tram *tram)
{
    m_currentTram = tram;
    m_currentNode->removePassenger(time, this);
    m_currentNode = nullptr;

    for (auto requestTram : m_entranceRequestsTrams)
        if (requestTram != tram)
            requestTram->revokePassengerEntranceRequest(this);

    m_entranceRequestsTrams.clear();

    //addHistoryRow(time);
}

void Passenger::exitTram(float time, RouteNode *node)
{
    m_currentTram = nullptr;
    m_currentNode = node;

    if (node == m_endNode)
    {
        delete this;
    }
    else
    {
        node->addPassenger(time, this);
    }
    //addHistoryRow(time);
}

void Passenger::addHistoryRow(float time)
{
    m_timeHistory.push_back(time);
    m_tramHistory.push_back(m_currentTram);
    m_nodeHistory.push_back(m_currentNode);
}