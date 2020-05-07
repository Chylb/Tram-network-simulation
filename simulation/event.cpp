#include "event.h"

#include "tram.h"
#include "simulation.h"
#include "edge.h"
#include "node.h"
#include "trafficLight.h"

Event::Event(float time)
{
	m_time = time;
}

void TramEvent::addNextEvent()
{
	auto nextEvent = m_tram->getNextEvent(m_time);
	nextEvent->m_simulation = m_simulation;
	m_simulation->addEvent(nextEvent);
}

TramEvent::TramEvent(Tram *tram, float time) : Event(time)
{
	m_tram = tram;
}

EventTramDeploy::EventTramDeploy(std::list<Node *> tripStops, std::list<TrafficLight *> tripTrafficLights, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation, int tramId) : TramEvent(nullptr, stopsTimes.front())
{
	m_firstEdge = tripPath.front();
	m_tram = new Tram(tramId, m_firstEdge);
	stopsTimes.pop_front();
	tripStops.pop_front();
	tripPath.pop_front();
	m_tram->setStopsToVisit(tripStops);
	m_tram->setTrafficLightsToVisit(tripTrafficLights);
	m_tram->setStopsTimes(stopsTimes);
	m_tram->setEdgesToVisit(tripPath);
}

void EventTramDeploy::processEvent()
{
	m_simulation->addTram(m_tram);
	m_firstEdge->addTram(m_tram);
	m_tram->addHistoryRow(m_time);

	addNextEvent();
}

EventReachedVmax::EventReachedVmax(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventReachedVmax::processEvent()
{
	m_tram->setSpeed(m_tram->getMaxSpeed());
	m_tram->changeState(Tram::moving, m_time);

	addNextEvent();
}

EventCheckForCollisions::EventCheckForCollisions(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventCheckForCollisions::processEvent()
{
	addNextEvent();
}

EventBeginDeceleration::EventBeginDeceleration(Tram *tram, float time, Node *target) : TramEvent(tram, time)
{
	m_target = target;
}

void EventBeginDeceleration::processEvent()
{
	float timeToNextEvent;
	auto nextEvent = m_tram->getEventAfterDeceleration(m_time, &timeToNextEvent, m_target);
	float nextEventPosition = m_tram->getPosition() + m_tram->stoppingDistance();
	nextEvent = m_tram->addIntermediateEvents(nextEvent, nextEventPosition, m_time, m_target);
	nextEvent->m_simulation = m_simulation;
	m_simulation->addEvent(nextEvent);
}

EventEndDeceleration::EventEndDeceleration(Tram *tram, float time, float targetSpeed) : TramEvent(tram, time)
{
	m_targetSpeed = targetSpeed;
}

void EventEndDeceleration::processEvent()
{
	m_tram->setSpeed(m_targetSpeed);

	if (m_targetSpeed == 0.0)
		m_tram->changeState(Tram::resting, m_time);
	else
		m_tram->changeState(Tram::moving, m_time);

	addNextEvent();
}

EventEnterNewEdge::EventEnterNewEdge(Tram *tram, float time) : TramEvent(tram, time)
{
	m_nextEvent = nullptr;
}

void EventEnterNewEdge::processEvent()
{
	m_tram->enterNextEdge(m_time);

	if (m_nextEvent == nullptr)
		m_nextEvent = m_tram->getNextEvent(m_time);

	m_nextEvent->m_simulation = m_simulation;
	m_simulation->addEvent(m_nextEvent);
}

EventPassangerExchange::EventPassangerExchange(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventPassangerExchange::processEvent()
{
	float stopDuration = m_tram->exchangePassengers(m_time);
	auto stopsToVisit = m_tram->getStopsToVisit();

	Event *event;
	if (stopsToVisit.size() > 1)
	{
		event = new EventEnterNewEdge(m_tram, m_time + stopDuration);
	}
	else
	{
		event = new EventEndTrip(m_tram, m_time + stopDuration);
	}

	event->m_simulation = m_simulation;
	m_simulation->addEvent(event);
}

EventWaitAtTrafficLights::EventWaitAtTrafficLights(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventWaitAtTrafficLights::processEvent()
{
	float waitDuration = m_tram->waitAtTrafficLights(m_time);

	Event *nextEvent = new EventEnterNewEdge(m_tram, m_time + waitDuration);

	nextEvent->m_simulation = m_simulation;
	m_simulation->addEvent(nextEvent);
}

EventEndTrip::EventEndTrip(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventEndTrip::processEvent()
{
	m_tram->getCurrentEdge()->removeTram();
	m_simulation->removeTram(m_tram);
}
