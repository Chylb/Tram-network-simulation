#include "trafficLight.h"

#include "tram.h"
#include "junction.h"

TrafficLight::TrafficLight(int id) : Node(id, true, false, false)
{
    m_junction = nullptr;
    changeState(false, 0.0);
}

void TrafficLight::setJunction(Junction *junction)
{
    m_junction = junction;
}

Junction *TrafficLight::getJunction()
{
    return m_junction;
}

bool TrafficLight::requestGreen(Tram *tram, float time)
{
    return m_junction->requestGreen(tram, this, time);
}

void TrafficLight::changeState(bool state, float time)
{
    m_stateHistory.push_back(state);
    m_timeHistory.push_back(time);
}

json TrafficLight::getHistory()
{
    json history;
    history["id"] = m_id;
    history["time"] = m_timeHistory;
    history["state"] = m_stateHistory;

    return history;
}