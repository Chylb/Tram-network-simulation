#include "tram.h"

//#include "edge.h"
//#include "node.h"

Tram::Tram()
{
    m_state = resting;
}

void Tram::setCurrentEdge(Edge *currentEdge)
{
    m_currentEdge = currentEdge;
}

void Tram::setEdgesToVisit(std::list<Edge *> edgesToVisit)
{
    m_edgesToVisit = edgesToVisit;
}

void Tram::setStopsToVisit(std::list<Node *> nodesToVisit)
{
    m_stopsToVisit = m_stopsToVisit;
}

void Tram::setStopsTimes(std::list<float> stopsTimes)
{
    m_stopsTimes = m_stopsTimes;
}

void Tram::updateStatistics(float dt)
{
    switch (m_state)
    {
    case resting:
        break;
    case accelerating:
        updateStatisticsAccelerating(dt);
        break;
    case decelerating:
        updateStatisticsDecelerating(dt);
        break;
    case moving:
        updateStatisticsMoving(dt);
        break;
    case exchangingPassangers:
        break;
    }
}

void Tram::updateStatisticsAccelerating(float dt)
{
    const float rate = 1.4;

    m_distance = m_distance + m_velocity * dt + 0.5 * rate * dt * dt;
    m_velocity = m_velocity + rate * dt;
}

void Tram::updateStatisticsDecelerating(float dt)
{
    const float rate = 1.4;

    m_distance = m_distance + m_velocity * dt - 0.5 * rate * dt * dt;
    m_velocity = m_velocity - rate * dt;
}

void Tram::updateStatisticsMoving(float dt)
{
    m_distance = m_distance + m_velocity * dt;
}
