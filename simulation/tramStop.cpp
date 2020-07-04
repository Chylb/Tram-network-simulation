#include "tramStop.h"

#include "routeNode.h"

TramStop::TramStop(int id, RouteNode *routeNode) : Node(id, false, false, true), m_routeNode(routeNode)
{
}

void TramStop::notifyPassengers(float time, Tram *tram)
{
    m_routeNode->notifyPassengers(time, tram);
}