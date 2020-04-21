#pragma once

#include <vector>
#include <list>

class Tram;
class Edge;
class Node;
class Simulation;

class Event
{

public:
    Event(float time);
    virtual void processEvent() = 0;
    //private:
    float m_time;

    struct CompareTime
    {
        bool operator()(Event *const &e1, Event *const &e2)
        {
            return e1->m_time > e2->m_time;
        }
    };
};

struct CompareEventTime
{
    bool operator()(Event *const &e1, Event *const &e2)
    {
        return e1->m_time > e2->m_time;
    }
};

class EventTramDeploy : public Event
{
public:
    EventTramDeploy(std::list<Node *> tripStops, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation);
    void processEvent();
};
