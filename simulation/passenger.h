#pragma once

#include <unordered_map>
#include <list>

#include "include/json.hpp"
using json = nlohmann::json;

class RouteNode;
class RouteEdge;
class Tram;
class TramStop;

class Passenger
{
public:
    Passenger(float time, RouteNode *startNode, RouteNode *endNode, std::unordered_map<RouteNode *, std::list<RouteEdge*>> *path);

    void notifyInside(float time, TramStop *tramStop);

    void enterTram(float time, Tram *tram);
    void exitTram(float time, RouteNode *routeNode);

    json getHistory();

private:
    void addHistoryRow(float time);

    RouteNode *m_currentNode;
    Tram *m_currentTram;
    RouteNode *m_endNode;
    std::unordered_map<RouteNode *, std::list<RouteEdge*>> *m_path;

    std::list<float> m_timeHistory;
    std::list<Tram *> m_tramHistory;
    std::list<RouteNode *> m_nodeHistory;
};