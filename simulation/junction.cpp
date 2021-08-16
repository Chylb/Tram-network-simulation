#include "junction.h"

#include "tram.h"
#include "trafficLight.h"

Junction::Junction()
{
	m_greenLight = nullptr;
}

void Junction::addTrafficLight(TrafficLight* trafficLight)
{
	m_trafficLights.push_back(trafficLight);
}

void Junction::addJunctionExit(Node* exit)
{
	m_exits.push_back(exit);
}

bool Junction::requestGreen(Tram* tram, TrafficLight* trafficLight, float time)
{
	if (m_greenLight == trafficLight)
	{
		m_tramsOnJunction.push_back(tram);
		return true;
	}

	if (m_greenLight == nullptr)
	{
		m_greenLight = trafficLight;
		trafficLight->changeState(true, time);
		m_tramsOnJunction.push_back(tram);
		return true;
	}

	m_requests.push_back(std::make_pair(tram, trafficLight));
	return false;
}

void Junction::removeTram(Tram* tram, float time)
{
	m_tramsOnJunction.remove(tram);
	if (m_tramsOnJunction.size() == 0)
	{
		m_greenLight->changeState(false, time);

		if (m_requests.size() == 0)
		{
			m_greenLight = nullptr;
		}
		else
		{
			m_greenLight = m_requests.front().second;
			m_greenLight->changeState(true, time);
			m_requests.front().first->notifyTrafficLight(time);
			m_tramsOnJunction.push_back(m_requests.front().first);

			m_requests.pop_front();
		}
	}
}