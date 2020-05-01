#include "simulation.h"

#include <iostream>

#include "graph.h"
#include "node.h"
#include "trafficLight.h"
#include "edge.h"
#include "tram.h"

Simulation::Simulation(json networkModel)
{
	for (json jNode : networkModel["nodes"])
	{
		int id = jNode["id"];

		Node *node;
		if (jNode.contains("trafficLight"))
		{
			node = new TrafficLight(id, jNode["trafficLight"]);
		}
		else if (jNode.contains("stopName"))
		{
			node = new Node(id, false, true);
		}
		else
			node = new Node(id);

		m_nodes[id] = node;
	}

	for (json jJunction : networkModel["junctions"])
	{
		for (int trafficLightId : jJunction["trafficLights"])
		{
			TrafficLight *trafficLight = (TrafficLight *)m_nodes[trafficLightId];
			trafficLight->setPhaseCount(jJunction["trafficLights"].size());
		}
	}

	for (json jEdge : networkModel["edges"])
	{
		int id = jEdge["id"];
		float length = jEdge["length"];
		float maxspeed = (float)jEdge["maxspeed"];

		Node *tail = m_nodes[jEdge["tail"]];
		Node *head = m_nodes[jEdge["head"]];

		Edge *edge = new Edge(id, tail, head, length, maxspeed);

		tail->addOutgoingEdge(edge);
		head->addIncomingEdge(edge);
	}

	std::vector<std::list<Edge *>> routesPaths;
	std::vector<std::list<TrafficLight *>> routesTrafficLights;

	for (json jRoute : networkModel["routes"])
	{
		int routeId = jRoute["id"];
		std::list<int> stops = jRoute["stops"];
		std::list<Edge *> routePath;
		std::list<TrafficLight *> routeTrafficLights;

		auto currentStop = stops.begin();
		auto nextStop = currentStop;
		nextStop++;

		while (nextStop != stops.end())
		{
			auto n1 = m_nodes[*currentStop];
			auto n2 = m_nodes[*nextStop];

			auto pathAndLights = Graph::findPath(n1, n2);
			auto path = pathAndLights.first;
			auto trafficLights = pathAndLights.second;
			routePath.splice(routePath.end(), path);
			routeTrafficLights.splice(routeTrafficLights.end(), trafficLights);

			currentStop = nextStop;
			++nextStop;
		}
		routesPaths.push_back(routePath);
		routesTrafficLights.push_back(routeTrafficLights);
	}

	int tramId = 0;

	for (json jTrip : networkModel["trips"])
	{
		int routeId = jTrip["route"];

		std::list<int> stopsIds = networkModel["routes"][routeId]["stops"];
		std::list<float> stopTimes = jTrip["times"];

		auto sIt = stopsIds.begin();
		for (auto tIt = stopTimes.begin(); tIt != stopTimes.end();)
		{
			if (*tIt == -1)
			{
				tIt = stopTimes.erase(tIt);
				sIt = stopsIds.erase(sIt);
			}
			else
			{
				++sIt;
				++tIt;
			}
		}

		std::transform(stopTimes.begin(), stopTimes.end(), stopTimes.begin(), [](float x) -> float { return 60 * x; }); //transforming minutes to seconds

		std::list<Node *> tripStops;
		for (auto sId : stopsIds)
			tripStops.push_back(m_nodes[sId]);

		auto tripPath = std::list<Edge *>(routesPaths[routeId]);
		auto tripTrafficLights = std::list<TrafficLight *>(routesTrafficLights[routeId]);

		while (tripPath.front()->getTail() != tripStops.front())
		{
			tripPath.pop_front();
			if (tripPath.front()->getTail()->isTrafficLight())
				tripTrafficLights.pop_front();
		}

		while (tripPath.back()->getHead() != tripStops.back())
		{
			tripPath.pop_back();
			if (tripPath.back()->getHead()->isTrafficLight())
				tripTrafficLights.pop_back();
		}

		auto event = new EventTramDeploy(tripStops, tripTrafficLights, stopTimes, tripPath, this, tramId);
		event->m_simulation = this;
		addEvent(event);

		tramId++;
	}
}

void Simulation::run()
{
	float time;

	while (!m_eventQueue.empty())
	{
		auto event = m_eventQueue.top();
		m_eventQueue.pop();

		time = event->m_time;

		for (auto tram : m_trams)
			tram->updateStatistics(time);

		//std::cout << time << std::endl;

		if (time > 40000)
		{
			return;
		}

		event->processEvent();
		delete event;
	}
}

void Simulation::addTram(Tram *tram)
{
	m_trams.push_back(tram);
}

void Simulation::removeTram(Tram *tram)
{
	m_trams.remove(tram);
	m_removedTrams.push_back(tram);
}

void Simulation::addEvent(Event *event)
{
	m_eventQueue.push(event);
}

json Simulation::getResults()
{
	json results;
	results["simulation"] = "yes";

	for (auto tram : m_removedTrams)
		results["trams"].push_back(tram->getHistory());

	for (auto tram : m_trams)
		results["trams"].push_back(tram->getHistory());

	return results;
}
