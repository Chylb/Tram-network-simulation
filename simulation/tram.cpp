#include "tram.h"

#include <cmath>
#include <algorithm>

#include "edge.h"
#include "event.h"
#include "trafficLight.h"
#include "junction.h"
#include "simulation.h"
#include "passenger.h"
#include "tramStop.h"
#include "routeNode.h"

Tram::Tram(int id, Edge* edge, Simulation* simulation)
{
	m_id = id;
	m_simulation = simulation;
	m_currentEdge = edge;
	m_state = resting;
	m_position = 0.0;
	m_speed = 0.0;
	m_nextEvent = nullptr;
	m_currentJunction = nullptr;
	m_requestedGreenLight = false;
	m_waitingTram = nullptr;
}

void Tram::setNextEvent(Event* event)
{
	m_nextEvent = event;
	m_simulation->addEvent(event);
}

void Tram::generateNextEvent(float time)
{
	enum EventCause
	{
		tramStop,
		trafficLight,
		tram,
		reachingVmax
	};
	EventCause eventCause;
	Node* eventCauseNode = nullptr;

	if (m_state == exchangingPassangers) //sometimes events happens while exchanging passengers, it prevents unexpected behaviour
		return;

	float nearestEventCausePosition = getNextStopPosition();
	eventCauseNode = m_stopsToVisit.front();
	eventCause = tramStop;

	float nextTrafficLightPosition = getNextTrafficLightPosition();
	if (nextTrafficLightPosition < nearestEventCausePosition)
	{
		eventCause = trafficLight;
		nearestEventCausePosition = nextTrafficLightPosition;
		eventCauseNode = (Node*)m_trafficLightsToVisit.front();
	}

	float nextTramPosition = getNextTramPosition(nearestEventCausePosition) - c_length;

	if (nextTramPosition < nearestEventCausePosition)
	{
		eventCause = tram;
		nearestEventCausePosition = nextTramPosition;
		eventCauseNode = nullptr;
		auto tramAhead = getTramAhead(INFINITY);

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
	Event* nextEvent;

	if (eventCause == reachingVmax)
	{
		changeState(accelerating, time);
		timeToNextEvent = acc_t_of_x(nextEventPosition - m_position, m_speed);

		nextEvent = new EventReachedVmax(this, time + timeToNextEvent);
	}
	else
	{
		if (m_state == moving)
			timeToNextEvent = (decelerationX0 - m_position) / m_speed;
		else
			timeToNextEvent = accdec_t1_of_x(nearestEventCausePosition - m_position, m_speed, 0.0);

		if (timeToNextEvent > 0.5)
		{
			if (m_state != moving)
				changeState(accelerating, time);

			nextEvent = new EventCheckForCollisions(this, time + timeToNextEvent);
		}
		else
		{
			timeToNextEvent = 0.0;

			if (m_state == resting)
			{
				if (eventCause == tram)
				{
					auto tramAhead = getTramAhead(INFINITY);
					tramAhead->setWaitingTram(this);
					nextEvent = nullptr;
				}
				else if (eventCause == trafficLight)
				{
					if (!m_requestedGreenLight)
					{
						TrafficLight* trafficLight = (TrafficLight*)eventCauseNode;

						if (trafficLight->requestGreen(this, time))
						{
							m_trafficLightsToVisit.pop_front();
							return generateNextEvent(time);
						}
						else
						{
							m_requestedGreenLight = true;
						}
					}
					nextEvent = nullptr;
				}
				else
				{
					nextEvent = new EventBeginPassangerExchange(this, time); //it is sometimes needed
				}
			}
			else
			{
				if (eventCause == trafficLight)
				{
					TrafficLight* trafficLight = (TrafficLight*)eventCauseNode;

					if (!m_requestedGreenLight)
					{
						if (trafficLight->requestGreen(this, time))
						{
							m_trafficLightsToVisit.pop_front();
							return generateNextEvent(time);
						}
						else
						{
							m_requestedGreenLight = true;
						}
					}
				}

				changeState(decelerating, time);
				timeToNextEvent = dec_t_of_v(0.0, m_speed);

				if (eventCause == tramStop)
					nextEvent = new EventBeginPassangerExchange(this, time + timeToNextEvent);
				else
					nextEvent = new EventEndDeceleration(this, time + timeToNextEvent, 0.0);
			}
		}
	}

	if (nextEvent == nullptr)
	{
		m_nextEvent = nullptr;
		return;
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

	nextEvent = addIntermediateEvents(nextEvent, nextEventPosition, time, eventCauseNode);
	m_nextEvent = nextEvent;
	m_simulation->addEvent(nextEvent);
}

Event* Tram::addIntermediateEvents(Event* mainEvent, float mainEventPosition, float time, Node* eventCauseNode)
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
		EventEnterNewEdge* lastEvent = firstEvent;

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

	if (m_currentEdge->getTail() == m_trafficLightsToVisit.front())
		return 0;

	float position = m_currentEdge->getLength();

	if (m_currentEdge->getHead() == m_trafficLightsToVisit.front())
		return position;

	auto it = m_edgesToVisit.begin();

	while ((*it)->getHead() != (Node*)m_trafficLightsToVisit.front())
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
	Tram* tramAhead = m_currentEdge->getTramAhead(this);
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

Tram* Tram::getTramAhead(float limit)
{
	Tram* tramAhead = m_currentEdge->getTramAhead(this);
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

	if (m_currentEdge->getTail()->isJunctionExit())
	{
		if (m_currentJunction != nullptr)
		{
			m_currentJunction->removeTram(this, time);
			m_currentJunction = nullptr;
		}
	}
	else if (m_currentEdge->getTail()->isTrafficLight())
	{
		m_currentJunction = ((TrafficLight*)m_currentEdge->getTail())->getJunction();
	}

	m_x0 = 0;
	m_v0 = m_speed;
	m_t0 = time;

	addHistoryRow(time);
}

void Tram::beginPassengerExchange(float time)
{
	m_speed = 0.0;
	changeState(exchangingPassangers, time);

	auto tramStop = m_stopsToVisit.front();

	for (auto passenger : m_passengers)
	{
		passenger->notifyInside(time, tramStop);
	}

	auto event = new EventPassangerExchangeUpdate(this, time + c_doorOpeningTime);
	setNextEvent(event);
}

void Tram::updatePassengerExchange(float time)
{
	auto node = m_stopsToVisit.front()->m_routeNode;
	Event* event;
	if (m_exitingPassengers.size() > 0)
	{
		auto passenger = m_exitingPassengers.front();
		m_exitingPassengers.pop_front();
		passenger->exitTram(time, node);
		m_passengers.remove(passenger);
		addPassengerHistoryRow(time);

		event = new EventPassangerExchangeUpdate(this, time + c_passengerExitingTime);
	}
	else {
		Passenger* passenger = node->dispensePassenger(time, m_route);
		if (m_passengers.size() < c_passengerCapacity && passenger)
		{
			passenger->enterTram(time, this);
			m_passengers.push_back(passenger);
			addPassengerHistoryRow(time);

			event = new EventPassangerExchangeUpdate(this, time + c_passengerEnteringTime);
		}
		else if (time < m_stopsTimes.front())
		{
			event = new EventPassangerExchangeUpdate(this, m_stopsTimes.front());
		}
		else
		{
			m_stopsTimes.pop_front();
			m_stopsToVisit.pop_front();

			changeState(resting, time);

			if (m_stopsToVisit.size() > 0)
				generateNextEvent(time + c_doorOpeningTime);
			else
				endTrip(time);
			return;
		}
	}
	setNextEvent(event);
}

void Tram::requestPassengerExit(Passenger* passenger)
{
	m_exitingPassengers.push_back(passenger);
}

void Tram::notifyTrafficLight(float time)
{
	m_trafficLightsToVisit.pop_front();
	m_requestedGreenLight = false;
	if (m_nextEvent != nullptr)
		m_simulation->removeEvent(m_nextEvent);

	generateNextEvent(time);
}

void Tram::notifyTram(float time)
{
	auto nextEvent = new EventCheckForCollisions(this, time + 5.0);
	m_simulation->addEvent(nextEvent);
	m_nextEvent = nextEvent;
}

void Tram::endTrip(float time)
{
	if (m_currentJunction != nullptr)
	{
		m_currentJunction->removeTram(this, time);
	}
	if (m_waitingTram != nullptr)
	{
		m_waitingTram->notifyTram(time);
	}
	getCurrentEdge()->removeTram();
	m_simulation->removeTram(this);
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
	if (x <= 0.0)
		return 0.0;
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
	if (x <= 0.0)
		return 0.0;
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
	m_position = m_x0 + acc_x_of_t(time - m_t0, m_v0);
	m_speed = acc_v_of_t(time - m_t0, m_v0);
}

void Tram::updateStatisticsDecelerating(float time)
{
	m_position = m_x0 + dec_x_of_t(time - m_t0, m_v0);
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

float Tram::getMaxDecelerationDistance()
{
	return c_decMaxX;
}

float Tram::getLength()
{
	return c_length;
}

Edge* Tram::getCurrentEdge()
{
	return m_currentEdge;
}

std::list<TramStop*> Tram::getStopsToVisit()
{
	return m_stopsToVisit;
}

float Tram::stoppingDistance()
{
	float t = dec_t_of_v(0.0, m_speed);
	return dec_x_of_t(t, m_speed);
}

int Tram::getRoute()
{
	return m_route;
}

void Tram::setCurrentEdge(Edge* currentEdge)
{
	m_currentEdge = currentEdge;
}

void Tram::setWaitingTram(Tram* waitingTram)
{
	m_waitingTram = waitingTram;
}

void Tram::setEdgesToVisit(std::list<Edge*> edgesToVisit)
{
	m_edgesToVisit = edgesToVisit;
}

void Tram::setStopsToVisit(std::list<TramStop*> nodesToVisit)
{
	m_stopsToVisit = nodesToVisit;
}

void Tram::setTrafficLightsToVisit(std::list<TrafficLight*> trafficLightsToVisit)
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

void Tram::setRoute(int route)
{
	m_route = route;
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

		if (m_waitingTram != nullptr)
		{
			m_waitingTram->notifyTram(time);
			m_waitingTram = nullptr;
		}
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

void Tram::addPassengerHistoryRow(float time)
{
	m_timePassengerHistory.push_back(time);
	m_passengerHistory.push_back(m_passengers.size());
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

	history["passengerTime"] = m_timePassengerHistory;
	history["passengers"] = m_passengerHistory;

	return history;
}

//////////////////////////////////////////////////////////////////////////

TramCollisionException::TramCollisionException(Tram* tram, Tram* tramAhead)
{
	m_msg = "Tram collision exception. Tram " + std::to_string(tram->getId()) + " hit into " + std::to_string(tramAhead->getId()) + ".";
}
