#include "event.h"

#include "tram.h"
#include "simulation.h"
#include "edge.h"
#include "node.h"
#include "trafficLight.h"
#include "graph.h"
#include "junction.h"
#include "routeNode.h"
#include "tramStop.h"
#include "passenger.h"

Event::Event(float time)
{
	m_time = time;
	m_requiresTramsUpdate = false;
}

TramEvent::TramEvent(Tram *tram, float time) : Event(time)
{
	m_tram = tram;
	m_requiresTramsUpdate = true;
}

EventTramDeploy::EventTramDeploy(std::list<TramStop *> tripStops, std::list<TrafficLight *> tripTrafficLights, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation, int tramId, int route) : TramEvent(nullptr, stopsTimes.front())
{
	m_simulation = simulation;

	auto firstEdge = tripPath.front();
	tripPath.pop_front();
	m_tram = new Tram(tramId, firstEdge, m_simulation);
	m_tram->setStopsToVisit(tripStops);
	m_tram->setTrafficLightsToVisit(tripTrafficLights);
	m_tram->setStopsTimes(stopsTimes);
	m_tram->setEdgesToVisit(tripPath);
	m_tram->setRoute(route);
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
	Tram* tramAhead = nullptr;
	if(m_tram->getCurrentEdge()->getTrams().size() > 0)
		tramAhead = m_tram->getCurrentEdge()->getTrams().front(); //function getTramAhead doesn't work on a first edge because the tram is not added yet
	if (tramAhead == nullptr)
		tramAhead = m_tram->getTramAhead(m_tram->getMaxDecelerationDistance());

	float distanceToTramAhead = INFINITY;
	if (tramAhead != nullptr)
		distanceToTramAhead = tramAhead->getPosition();
	float distanceToTramBehind = Graph::nearestTramBehindDistance(m_tram->getCurrentEdge()->getTail(), m_tram->getMaxDecelerationDistance());

	if (m_tram->getLength() < distanceToTramAhead && m_tram->getMaxDecelerationDistance() < distanceToTramBehind)
	{
		m_simulation->addTram(m_tram);
		m_tram->getCurrentEdge()->addTram(m_tram);
		m_tram->addHistoryRow(m_time);
		m_tram->addPassengerHistoryRow(m_time);
		m_tram->beginPassengerExchange(m_time);
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

EventBeginPassangerExchange::EventBeginPassangerExchange(Tram *tram, float time) : TramEvent(tram, time)
{
}

void EventBeginPassangerExchange::processEvent()
{
	m_tram->beginPassengerExchange(m_time);
}

EventPassangerExchangeUpdate::EventPassangerExchangeUpdate(Tram *tram, float time) : TramEvent(tram, time)
{
	m_requiresTramsUpdate = false;
}

void EventPassangerExchangeUpdate::processEvent()
{
	m_tram->updatePassengerExchange(m_time);
}

EventSpawnPassenger::EventSpawnPassenger(float time, Simulation* simulation, RouteNode *startNode, RouteNode *endNode, std::unordered_map<RouteNode *, std::list<RouteEdge*>> *path) : Event(time)
{
	m_simulation = simulation;
	m_startNode = startNode;
	m_endNode = endNode;
	m_path = path;
}

void EventSpawnPassenger::processEvent()
{
	m_simulation->addPassenger(m_time, m_startNode, m_endNode, m_path);
}
