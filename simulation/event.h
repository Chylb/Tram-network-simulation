#pragma once

#include <vector>
#include <list>

class Tram;
class Edge;
class Node;
class Simulation;
class TrafficLight;

struct Event
{
    Event(float time);

    virtual void processEvent() = 0;
    
    float m_time;
    Simulation *m_simulation;

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
    void addNextEvent();

    Tram *m_tram;
};

struct EventTramDeploy : public TramEvent
{
    EventTramDeploy(std::list<Node *> tripStops, std::list<TrafficLight *> tripTrafficLights, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation, int tramId);
    Edge *m_firstEdge;
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

struct EventBeginDeceleration : public TramEvent
{
    EventBeginDeceleration(Tram *tram, float time, Node *target);
    void processEvent();
    Node *m_target;
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

struct EventPassangerExchange : public TramEvent
{
    EventPassangerExchange(Tram *tram, float time);
    void processEvent();
};

struct EventWaitAtTrafficLights : public TramEvent
{
    EventWaitAtTrafficLights(Tram *tram, float time);
    void processEvent();
};

struct EventEndTrip : public TramEvent
{
    EventEndTrip(Tram *tram, float time);
    void processEvent();
};
