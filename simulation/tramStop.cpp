#include "tramStop.h"

#include "routeNode.h"

TramStop::TramStop(int id, RouteNode *routeNode) : Node(id, false, false, true), m_routeNode(routeNode)
{
}