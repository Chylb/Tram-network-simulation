#include "passenger.h"

#include "graph.h"
#include "tram.h"
#include "routeNode.h"
#include "tramStop.h"

#include <iostream>

Passenger::Passenger(float time, RouteNode *startNode, RouteNode *endNode)
{
    m_endNode = endNode;
    m_path = Graph::findPassengerPath(startNode, endNode);
    m_currentNode = startNode;
    startNode->addPassenger(time, this);
}

void Passenger::notifyOutside(float time, Tram *tram)
{
    for (int route : m_path[m_currentNode])
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
    for (int route : m_path[tramStop->m_routeNode])
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

    for (auto requestTram : m_entranceRequestsTrams)
        if (requestTram != tram)
            requestTram->revokePassengerEntranceRequest(this);

    m_entranceRequestsTrams.clear();
}

void Passenger::exitTram(float time, RouteNode *node)
{
    m_currentTram = nullptr;
    m_currentNode = node;
    node->addPassenger(time, this);
    if(node == m_endNode) {
        node->removePassenger(time, this);
    }
}

void Passenger::addHistoryRow(float time)
{
}