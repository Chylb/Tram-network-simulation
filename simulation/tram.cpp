#include "tram.h"

#include "edge.h"
#include "event.h"
#include "trafficLight.h"

#include <cmath>

Tram::Tram(int id, Edge *edge)
{
    m_id = id;
    m_currentEdge = edge;
    m_state = resting;
    m_position = 0.0;
    m_speed = 0.0;
}

Event *Tram::getNextEvent(float time)
{
    enum EventCause
    {
        tramStop,
        trafficLight,
        tram,
        reachingVmax
    };
    EventCause eventCause;
    Node *eventCauseNode = nullptr;

    float nearestEventCausePosition = getNextStopPosition();
    eventCauseNode = m_stopsToVisit.front();
    eventCause = tramStop;

    float nextTrafficLightPosition = getNextTrafficLightPosition();
    if (nextTrafficLightPosition < nearestEventCausePosition)
    {
        eventCause = trafficLight;
        nearestEventCausePosition = nextTrafficLightPosition;
        eventCauseNode = (Node *)m_trafficLightsToVisit.front();
    }

    float nextTramPosition = getNextTramPosition(nearestEventCausePosition) - c_length;
    if (nextTramPosition < nearestEventCausePosition)
    {
        eventCause = tram;
        nearestEventCausePosition = nextTramPosition;
        eventCauseNode = nullptr;

        if (nextTramPosition - m_position - stoppingDistance() < -c_length + 1.0)
        {
            auto tramAhead = getTramAhead(INFINITY);
            throw TramCollisionException(this, tramAhead);
        }
    }

    float vMaxPosition = INFINITY;
    if (m_speed < c_maxSpeed)
    {
        float accelerationTime = acc_t_of_v(c_maxSpeed, m_speed);
        vMaxPosition = m_position + acc_x_of_t(accelerationTime, m_speed);
    }

    float decelerationX0 = nearestEventCausePosition - c_decMaxX;
    float nextEventPosition = decelerationX0;

    if (vMaxPosition < nextEventPosition)
    {
        nextEventPosition = vMaxPosition;
        eventCause = reachingVmax;
        eventCauseNode = nullptr;
    }

    float timeToNextEvent;
    Event *nextEvent;

    if (eventCause == reachingVmax)
    {
        changeState(accelerating, time);
        timeToNextEvent = acc_t_of_x(nextEventPosition - m_position, m_speed);

        nextEvent = new EventReachedVmax(this, time + timeToNextEvent);
    }
    else if (m_state == moving)
    {
        timeToNextEvent = (decelerationX0 - m_position) / m_speed;
        if (timeToNextEvent < 0.5)
            timeToNextEvent = 0.0;

        if (timeToNextEvent == 0.0)
        {
            if (eventCause == trafficLight)
            {
                auto trafficLight = m_trafficLightsToVisit.front();
                if (trafficLight->timeToGreen(time) == 0.0)
                {
                    m_trafficLightsToVisit.pop_front();
                    return getNextEvent(time);
                }
            }
            nextEvent = getEventAfterDeceleration(time, &timeToNextEvent, eventCauseNode);
        }
        else
        {
            nextEvent = new EventCheckForCollisions(this, time + timeToNextEvent);
        }
    }
    else
    {
        timeToNextEvent = accdec_t1_of_x(nearestEventCausePosition - m_position, m_speed, 0.0);

        if (timeToNextEvent < 0.5)
        {
            timeToNextEvent = 0.0;

            if (m_state == resting)
            {
                if (eventCauseNode != nullptr)
                {
                    nextEvent = getEventAfterDeceleration(time, &timeToNextEvent, eventCauseNode);
                }
                else
                {
                    nextEvent = new EventCheckForCollisions(this, time + 10.0);
                }
            }
            else
            {
                if (eventCause == trafficLight)
                {
                    auto trafficLight = m_trafficLightsToVisit.front();
                    if (trafficLight->timeToGreen(time) == 0.0)
                    {
                        m_trafficLightsToVisit.pop_front();
                        return getNextEvent(time);
                    }
                }
                nextEvent = getEventAfterDeceleration(time, &timeToNextEvent, eventCauseNode);
            }
        }
        else
        {
            changeState(accelerating, time);

            if (eventCause == tramStop)
            {
                nextEvent = new EventBeginDeceleration(this, time + timeToNextEvent, eventCauseNode);
            }
            else
            {
                nextEvent = new EventCheckForCollisions(this, time + timeToNextEvent);
            }
        }
    }

    switch (m_state)
    {
    case accelerating:
        nextEventPosition = m_position + acc_x_of_t(timeToNextEvent, m_speed);
        break;
    case decelerating:
        nextEventPosition = m_position + dec_x_of_t(timeToNextEvent, m_speed);
        break;
    case moving:
        nextEventPosition = m_position + timeToNextEvent * m_speed;
        break;
    default:
        nextEventPosition = m_position;
        break;
    }

    return addIntermediateEvents(nextEvent, nextEventPosition, time, eventCauseNode);
}

Event *Tram::addIntermediateEvents(Event *mainEvent, float mainEventPosition, float time, Node *eventCauseNode)
{
    if (m_currentEdge->getLength() < mainEventPosition && m_currentEdge->getHead() != eventCauseNode)
    {
        float timeToReachEdge;
        if (m_state == accelerating)
        {
            timeToReachEdge = acc_t_of_x(m_currentEdge->getLength() - m_position, m_speed);
        }
        else if (m_state == decelerating)
        {
            timeToReachEdge = dec_t_of_x(m_currentEdge->getLength() - m_position, m_speed);
        }
        else
        {
            timeToReachEdge = (m_currentEdge->getLength() - m_position) / m_speed;
        }
        auto firstEvent = new EventEnterNewEdge(this, time + timeToReachEdge);
        EventEnterNewEdge *lastEvent = firstEvent;

        float totalLength = m_currentEdge->getLength() + m_edgesToVisit.front()->getLength();
        auto it = m_edgesToVisit.begin();

        while (totalLength < mainEventPosition && (*it)->getHead() != eventCauseNode)
        {
            if (m_state == accelerating)
            {
                timeToReachEdge = acc_t_of_x(totalLength - m_position, m_speed);
            }
            else if (m_state == decelerating)
            {
                timeToReachEdge = dec_t_of_x(totalLength - m_position, m_speed);
            }
            else
            {
                timeToReachEdge = (totalLength - m_position) / m_speed;
            }

            auto intermediateEvent = new EventEnterNewEdge(this, time + timeToReachEdge);
            lastEvent->m_nextEvent = intermediateEvent;
            lastEvent = intermediateEvent;

            it++;
            totalLength += (*it)->getLength();
        }

        lastEvent->m_nextEvent = mainEvent;
        return firstEvent;
    }
    return mainEvent;
}

Event *Tram::getEventAfterDeceleration(float time, float *timeToNextEvent, Node *eventCauseNode)
{
    changeState(decelerating, time);
    *timeToNextEvent = dec_t_of_v(0.0, m_speed);

    Event *event;
    if (eventCauseNode != nullptr)
    {
        if (eventCauseNode->isTramStop())
        {
            event = new EventPassangerExchange(this, time + *timeToNextEvent);
        }
        else if (eventCauseNode->isTrafficLight())
        {
            event = new EventWaitAtTrafficLights(this, time + *timeToNextEvent);
        }
    }
    else
    {
        event = new EventEndDeceleration(this, time + *timeToNextEvent, 0.0);
    }
    return event;
}

float Tram::getNextStopPosition()
{
    float position = m_currentEdge->getLength();

    if (m_currentEdge->getHead() == m_stopsToVisit.front())
        return position;

    auto it = m_edgesToVisit.begin();

    while ((*it)->getHead() != m_stopsToVisit.front())
    {
        auto edge = (*it);
        position += (*it)->getLength();
        ++it;
    }

    position += (*it)->getLength();
    return position;
}

float Tram::getNextTrafficLightPosition()
{
    if (m_trafficLightsToVisit.size() == 0)
        return INFINITY;

    float position = m_currentEdge->getLength();

    if (m_currentEdge->getHead() == m_trafficLightsToVisit.front())
        return position;

    auto it = m_edgesToVisit.begin();

    while ((*it)->getHead() != (Node *)m_trafficLightsToVisit.front())
    {
        auto edge = (*it);
        position += (*it)->getLength();
        ++it;
    }

    position += (*it)->getLength();
    return position;
}

float Tram::getNextTramPosition(float limit)
{
    Tram *tramAhead = m_currentEdge->getTramAhead(this);
    if (tramAhead != nullptr)
        return tramAhead->getPosition();
    else
    {
        float totalLength = m_currentEdge->getLength();
        auto it = m_edgesToVisit.begin();

        while (it != m_edgesToVisit.end() && (*it)->getTrams().size() == 0 && totalLength < limit + c_length + 1.0)
        {
            totalLength += (*it)->getLength();
            ++it;
        }

        if (it != m_edgesToVisit.end() && (*it)->getTrams().size() > 0)
        {
            auto edge = (*it);
            return totalLength + (*it)->getTrams().front()->getPosition();
        }
        else
            return INFINITY;
    }
}

Tram *Tram::getTramAhead(float limit)
{
    Tram *tramAhead = m_currentEdge->getTramAhead(this);
    if (tramAhead != nullptr)
        return tramAhead;
    else
    {
        float totalLength = m_currentEdge->getLength();
        auto it = m_edgesToVisit.begin();

        while (it != m_edgesToVisit.end() && (*it)->getTrams().size() == 0 && totalLength < limit + c_length + 1.0)
        {
            totalLength += (*it)->getLength();
            ++it;
        }

        if (it != m_edgesToVisit.end() && (*it)->getTrams().size() > 0)
        {
            auto edge = (*it);
            return (*it)->getTrams().front();
        }
        else
            return nullptr;
    }
}

void Tram::enterNextEdge(float time)
{
    m_position = 0;
    m_currentEdge->removeTram();
    m_currentEdge = m_edgesToVisit.front();
    m_edgesToVisit.pop_front();
    m_currentEdge->addTram(this);

    m_x0 = 0;
    m_v0 = m_speed;
    m_t0 = time;

    addHistoryRow(time);
}

float Tram::exchangePassengers(float time)
{
    changeState(Tram::resting, time);
    m_position = m_currentEdge->getLength();
    m_speed = 0.0;

    float stopDuration = m_stopsTimes.front() - time;
    if (stopDuration < 10.0)
        stopDuration = 10.0;

    m_stopsToVisit.pop_front();
    m_stopsTimes.pop_front();

    return stopDuration;
}

float Tram::waitAtTrafficLights(float time)
{
    changeState(Tram::resting, time);
    m_position = m_currentEdge->getLength();
    m_speed = 0;

    float waitDuration = m_trafficLightsToVisit.front()->timeToGreen(time);
    m_trafficLightsToVisit.pop_front();

    return waitDuration;
}

//////////////////////////////////////////////////////////////

float Tram::acc_v_of_t(float t, float v0)
{
    return 1.4 * t + v0;
}
float Tram::acc_t_of_v(float v, float v0)
{
    return (v - v0) / 1.4;
}
float Tram::acc_x_of_t(float t, float v0)
{
    return v0 * t + 0.5 * 1.4 * t * t;
}
float Tram::acc_t_of_x(float x, float v0)
{
    float delta = v0 * v0 + 2.0 * 1.4 * x;
    float t = fabs((-v0 + sqrt(delta)) / (1.4));
    if (std::isnan(t))
        t = 0.0;
    return t;
}

float Tram::dec_v_of_t(float t, float v0)
{
    return -1.4 * t + v0;
}
float Tram::dec_t_of_v(float v, float v0)
{
    return (v0 - v) / 1.4;
}
float Tram::dec_x_of_t(float t, float v0)
{
    return v0 * t - 0.5 * 1.4 * t * t;
}
float Tram::dec_t_of_x(float x, float v0)
{
    float delta = v0 * v0 - 2.0 * 1.4 * x;
    float t = fabs((-v0 + sqrt(delta)) / (1.4));
    if (std::isnan(t))
        t = 0.0;
    return t;
}
float Tram::accdec_t1_of_x(float x, float v0, float v2)
{
    if (v0 > v2)
    {
        float tp = dec_t_of_v(v2, v0);
        float xp = dec_x_of_t(tp, v0);
        return acc_t_of_x((x - xp) / 2.0, v0);
    }
    else
    {
        float tp = acc_t_of_v(v2, v0);
        float xp = acc_x_of_t(tp, v0);
        return acc_t_of_x((x - xp) / 2.0, v0) + tp;
    }
}

//////////////////////////////////////////////////////////////

void Tram::updateStatistics(float time)
{
    switch (m_state)
    {
    case resting:
        break;
    case accelerating:
        updateStatisticsAccelerating(time);
        break;
    case decelerating:
        updateStatisticsDecelerating(time);
        break;
    case moving:
        updateStatisticsMoving(time);
        break;
    case exchangingPassangers:
        break;
    }
}

void Tram::updateStatisticsAccelerating(float time)
{
    m_position = m_position = m_x0 + acc_x_of_t(time - m_t0, m_v0);
    m_speed = acc_v_of_t(time - m_t0, m_v0);
}

void Tram::updateStatisticsDecelerating(float time)
{
    m_position = m_position = m_x0 + dec_x_of_t(time - m_t0, m_v0);
    m_speed = dec_v_of_t(time - m_t0, m_v0);
}

void Tram::updateStatisticsMoving(float time)
{
    m_position = m_x0 + (time - m_t0) * m_v0;
}

//////////////////////////////////////////////////////////////
int Tram::getId()
{
    return m_id;
}

float Tram::getPosition()
{
    return m_position;
}

float Tram::getMaxSpeed()
{
    return c_maxSpeed;
}

Edge *Tram::getCurrentEdge()
{
    return m_currentEdge;
}

std::list<Node *> Tram::getStopsToVisit()
{
    return m_stopsToVisit;
}

float Tram::stoppingDistance()
{
    float t = dec_t_of_v(0.0, m_speed);
    return dec_x_of_t(t, m_speed);
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
    m_stopsToVisit = nodesToVisit;
}

void Tram::setTrafficLightsToVisit(std::list<TrafficLight *> trafficLightsToVisit)
{
    m_trafficLightsToVisit = trafficLightsToVisit;
}

void Tram::setStopsTimes(std::list<float> stopsTimes)
{
    m_stopsTimes = stopsTimes;
}

void Tram::setSpeed(float speed)
{
    m_speed = speed;
}

void Tram::changeState(State state, float time)
{
    if (state != m_state)
    {
        m_state = state;

        addHistoryRow(time);

        m_x0 = m_position;
        m_v0 = m_speed;
        m_t0 = time;
    }
}

void Tram::addHistoryRow(float time)
{
    m_stateHistory.push_back(m_state);
    m_timeHistory.push_back(time);
    m_positionHistory.push_back(m_position);
    m_speedHistory.push_back(m_speed);
    m_edgeHistory.push_back(m_currentEdge->getId());
}

json Tram::getHistory()
{
    json history;
    history["id"] = m_id;
    history["time"] = m_timeHistory;
    history["state"] = m_stateHistory;
    history["position"] = m_positionHistory;
    history["speed"] = m_speedHistory;
    history["edge"] = m_edgeHistory;

    return history;
}

//////////////////////////////////////////////////////////////////////////

TramCollisionException::TramCollisionException(Tram *tram, Tram* tramAhead)
{
    m_msg = "Tram collision exception. Tram " + std::to_string(tram->getId()) + " hit into " + std::to_string(tramAhead->getId()) + ".";
}
