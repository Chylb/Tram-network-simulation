#include "event.h"

#include "tram.h"
#include "simulation.h"

Event::Event(float time)
{
	m_time = time;
}

EventTramDeploy::EventTramDeploy(std::list<Node *> tripStops, std::list<float> stopsTimes, std::list<Edge *> tripPath, Simulation *simulation) : Event(stopsTimes.front())
{
	auto tram = new Tram();
	tram->setStopsToVisit(tripStops);
	tram->setStopsTimes(stopsTimes);
	tram->setEdgesToVisit(tripPath);
	simulation->addTram(tram);
}

void EventTramDeploy::processEvent() {
	
}