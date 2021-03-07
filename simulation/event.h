#pragma once

#include <vector>
#include <list>
#include <unordered_map>

class Tram;
class Edge;
class Node;
class Simulation;
class TrafficLight;
class TramStop;
class RouteNode;
class RouteEdge;
class Passenger;

struct Event
{
    Event(float time);
    virtual void processEvent() = 0;

    float m_time;
    bool m_requiresTramsUpdate;

    struct CompareTime
    {
        bool operator()(Event *const &e1, Event *const &e2)
        {
            return e1->m_time > e2->m_time;
        }
    };
};

struct TramEvent : public Event
{
    TramEvent(Tram *tram, float time);
    Tram *m_tram;
};

struct EventTramDeploy : public TramEvent
{
    EventTramDeploy(std::list<TramStop *> tripStops, std::list<TrafficLight *> tripTrafficLights, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation, int tramId, int route);
    Simulation *m_simulation;
    void processEvent();
};

struct EventTramSpawn : public TramEvent
{
    EventTramSpawn(Tram *tram, float time, Simulation *simulation);
    Simulation *m_simulation;
    void processEvent();
};

struct EventReachedVmax : public TramEvent
{
    EventReachedVmax(Tram *tram, float time);
    void processEvent();
};

struct EventCheckForCollisions : public TramEvent
{
    EventCheckForCollisions(Tram *tram, float time);
    void processEvent();
};

struct EventEndDeceleration : public TramEvent
{
    EventEndDeceleration(Tram *tram, float time, float targetSpeed);
    void processEvent();
    float m_targetSpeed;
};

struct EventEnterNewEdge : public TramEvent
{
    EventEnterNewEdge(Tram *tram, float time);
    void processEvent();
    Event *m_nextEvent;
};

struct EventBeginPassangerExchange : public TramEvent
{
    EventBeginPassangerExchange(Tram *tram, float time);
    void processEvent();
};

struct EventPassangerExchangeUpdate : public TramEvent
{
    EventPassangerExchangeUpdate(Tram *tram, float time);
    void processEvent();
};

struct EventSpawnPassenger : public Event
{
    EventSpawnPassenger(float time, Simulation* simulation, RouteNode *startNode, RouteNode *endNode, std::unordered_map<RouteNode *, std::list<RouteEdge*>> *path);
    void processEvent();
    Simulation* m_simulation;
    RouteNode *m_startNode;
    RouteNode *m_endNode;
    std::unordered_map<RouteNode *, std::list<RouteEdge*>> *m_path;
};
