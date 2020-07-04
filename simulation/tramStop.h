#pragma once

#include "node.h"

class RouteNode;
class Tram;

class TramStop : public Node
{
public:
    TramStop(int id, RouteNode *routeNode);
    void notifyPassengers(float time, Tram *tram);

    RouteNode *const m_routeNode;
};