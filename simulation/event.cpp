#include "event.h"

#include "tram.h"
#include "simulation.h"
#include "edge.h"
#include "node.h"
#include "trafficLight.h"
#include "graph.h"
#include "junction.h"

Event::Event(float time)
{
	m_time = time;
}

TramEvent::TramEvent(Tram *tram, float time) : Event(time)
{
	m_tram = tram;
}

EventTramDeploy::EventTramDeploy(std::list<Node *> tripStops, std::list<TrafficLight *> tripTrafficLights, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation, int tramId) : TramEvent(nullptr, stopsTimes.front())
{
	m_simulation = simulation;

	auto firstEdge = tripPath.front()->getTail()->getIncomingEdges().front(); //temporary solution
	m_tram = new Tram(tramId, firstEdge, m_simulation);
	m_tram->setStopsToVisit(tripStops);
	m_tram->setTrafficLightsToVisit(tripTrafficLights);
	m_tram->setStopsTimes(stopsTimes);
	m_tram->setEdgesToVisit(tripPath);
}

void EventTramDeploy::processEvent()
{
	auto event = new EventTramSpawn(m_tram, m_time, m_simulation);
	m_simulation->addEvent(event);
}

EventTramSpawn::EventTramSpawn(Tram *tram, float time, Simulation *simulation) : TramEvent(tram, time)
{
	m_simulation = simulation;
}

void EventTramSpawn::processEvent()
{
	auto tramAhead = m_tram->getTramAhead(m_tram->getMaxDecelerationDistance());
	float distanceToTramAhead = INFINITY;
	if (tramAhead != nullptr)
		distanceToTramAhead = tramAhead->getPosition();
	float distanceToTramBehind = Graph::nearestTramBehindDistance(m_tram->getCurrentEdge()->getHead(), m_tram->getMaxDecelerationDistance());

	if (m_tram->getLength() < distanceToTramAhead && m_tram->getMaxDecelerationDistance() < distanceToTramBehind)
	{
		m_simulation->addTram(m_tram);
		m_tram->getCurrentEdge()->addTram(m_tram);
		m_tram->addHistoryRow(m_time);
		auto event = new EventPassangerExchange(m_tram, m_time);
		event->processEvent();
	}
	else
	{
		auto event = new EventTramSpawn(m_tram, m_time + 10.0, m_simulation);
		m_simulation->addEvent(event);
	}
}

EventReachedVmax::EventReachedVmax(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventReachedVmax::processEvent()
{
	m_tram->setSpeed(m_tram->getMaxSpeed());
	m_tram->changeState(Tram::moving, m_time);
	m_tram->generateNextEvent(m_time);
}

EventCheckForCollisions::EventCheckForCollisions(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventCheckForCollisions::processEvent()
{
	m_tram->generateNextEvent(m_time);
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

	m_tram->generateNextEvent(m_time);
}

EventEnterNewEdge::EventEnterNewEdge(Tram *tram, float time) : TramEvent(tram, time)
{
	m_nextEvent = nullptr;
}

void EventEnterNewEdge::processEvent()
{
	m_tram->enterNextEdge(m_time);

	if (m_nextEvent == nullptr)
		m_tram->generateNextEvent(m_time);
	else
		m_tram->setNextEvent(m_nextEvent);
}

EventPassangerExchange::EventPassangerExchange(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventPassangerExchange::processEvent()
{
	float stopDuration = m_tram->exchangePassengers(m_time);
	auto stopsToVisit = m_tram->getStopsToVisit();

	Event *event;
	if (stopsToVisit.size() > 0)
	{
		event = new EventEnterNewEdge(m_tram, m_time + stopDuration);
	}
	else
	{
		event = new EventEndTrip(m_tram, m_time + stopDuration);
	}

	m_tram->setNextEvent(event);
}

EventEndTrip::EventEndTrip(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventEndTrip::processEvent()
{
	m_tram->endTrip(m_time);
}
