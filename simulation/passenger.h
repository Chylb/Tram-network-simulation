#pragma once

#include <unordered_map>
#include <list>

#include "include/json.hpp"
using json = nlohmann::json;

class RouteNode;
class Tram;
class TramStop;

class Passenger
{
public:
    Passenger(float time, RouteNode *startNode, RouteNode *endNode);

    void notifyOutside(float time, Tram *tram);
    void notifyInside(float time, TramStop *tramStop);

    void enterTram(float time, Tram *tram);
    void exitTram(float time, RouteNode *routeNode);

    json getHistory();

private:
    void addHistoryRow(float time);

    RouteNode *m_currentNode;
    Tram *m_currentTram;
    RouteNode *m_endNode;
    std::unordered_map<RouteNode *, std::list<int>> m_path;

    std::list<Tram *> m_entranceRequestsTrams;

    std::list<Tram *> m_tramHistory;
    std::list<Tram *> m_timeHistory;
};