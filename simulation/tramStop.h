#pragma once

#include "node.h"

class RouteNode;
class Tram;

class TramStop : public Node
{
public:
    TramStop(int id, RouteNode *routeNode);

    RouteNode *const m_routeNode;
};