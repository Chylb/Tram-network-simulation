#include "simulation.h"

#include <iostream>

#include "graph.h"
#include "node.h"
#include "trafficLight.h"
#include "edge.h"
#include "tram.h"
#include "junction.h"
#include "routeNode.h"
#include "routeEdge.h"
#include "tramStop.h"

Simulation::Simulation(json networkModel)
{
	for (json jRouteNode : networkModel["routeNodes"])
	{
		std::string name = jRouteNode["name"];
		auto routeNode = new RouteNode(name);
		m_routeNodes[name] = routeNode;
	}

	for (json jRouteEdge : networkModel["routeEdges"])
	{
		auto head = m_routeNodes[jRouteEdge["head"]];
		auto tail = m_routeNodes[jRouteEdge["tail"]];
		std::list<int> lines = jRouteEdge["lines"];

		auto routeEdge = new RouteEdge(lines, tail, head);

		tail->addOutgoingEdge(routeEdge);
		head->addIncomingEdge(routeEdge);
	}

	m_routeNodeArray = new RouteNode *[m_routeNodes.size()];

	auto it = m_routeNodes.begin();
	for (int i = 0; i < m_routeNodes.size(); ++i)
	{
		m_routeNodeArray[i] = (*it).second;
		it++;
	}

	/////////////////////////////////////////////////

	for (json jNode : networkModel["nodes"])
	{
		int id = jNode["id"];

		Node *node;
		if (jNode.contains("trafficLight"))
		{
			node = new TrafficLight(id);
			m_trafficLights.push_back((TrafficLight *)node);
		}
		else if (jNode.contains("exit"))
		{
			node = new Node(id, false, true, false);
		}
		else if (jNode.contains("stopName"))
		{
			auto routeNode = m_routeNodes[jNode["stopName"]];
			node = new TramStop(id, routeNode);
		}
		else
			node = new Node(id);

		m_nodes[id] = node;
	}

	for (json jJunction : networkModel["junctions"])
	{
		auto junction = new Junction();

		for (int trafficLightId : jJunction["trafficLights"])
		{
			TrafficLight *trafficLight = (TrafficLight *)m_nodes[trafficLightId];
			trafficLight->setJunction(junction);
			junction->addTrafficLight(trafficLight);
		}

		for (int exitId : jJunction["exits"])
		{
			Node *exit = m_nodes[exitId];
			junction->addJunctionExit(exit);
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

			auto pathAndLights = Graph::findTramPath(n1, n2);
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

		std::list<TramStop *> tripStops;
		for (auto sId : stopsIds)
			tripStops.push_back((TramStop *)m_nodes[sId]);

		auto tripPath = std::list<Edge *>(routesPaths[routeId]);
		auto tripTrafficLights = std::list<TrafficLight *>(routesTrafficLights[routeId]);

		while (tripPath.front()->getTail() != (Node *)tripStops.front())
		{
			tripPath.pop_front();
			if (tripPath.front()->getTail()->isTrafficLight())
				tripTrafficLights.pop_front();
		}

		while (tripPath.back()->getHead() != (Node *)tripStops.back())
		{
			tripPath.pop_back();
			if (tripPath.back()->getHead()->isTrafficLight())
				tripTrafficLights.pop_back();
		}

		auto event = new EventTramDeploy(tripStops, tripTrafficLights, stopTimes, tripPath, this, tramId, routeId);
		addEvent(event);

		tramId++;
	}

	//generating passengers
	srand(0);
	for (int i = 0; i < 100000; ++i)
	{
		int ix1 = rand() % m_routeNodes.size();
		int ix2 = rand() % m_routeNodes.size();
		while (ix1 == ix2)
		{
			ix2 = rand() % m_routeNodes.size();
		}

		auto node1 = m_routeNodeArray[ix1];
		auto node2 = m_routeNodeArray[ix2];
		float time = 16000 + rand() % 70400;

		auto event = new EventSpawnPassenger(time, node1, node2);
		addEvent(event);
	}
}

void Simulation::run()
{
	float time;

	while (!m_eventQueue.empty())
	{
		auto event = m_eventQueue.top();
		m_eventQueue.pop();

		auto deletedEvent = m_removedEvents.find(event);
		if (deletedEvent != m_removedEvents.end())
		{
			m_removedEvents.erase(deletedEvent);
			delete event;
			continue;
		}

		time = event->m_time;

		if (event->m_requiresTramsUpdate)
			for (auto tram : m_trams)
			{
				tram->updateStatistics(time);
			}

		try
		{
			event->processEvent();
		}
		catch (const std::exception &exc)
		{
			std::cout << "Exception occured at " << time << "." << std::endl;
			std::cerr << exc.what() << std::endl;
			return;
		}

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

void Simulation::removeEvent(Event *event)
{
	if (event != nullptr)
	{
		m_removedEvents.insert(event);
	}
}

json Simulation::getResults()
{
	json results;
	results["simulation"] = "yes";

	for (auto tram : m_removedTrams)
		results["trams"].push_back(tram->getHistory());

	for (auto tram : m_trams)
		results["trams"].push_back(tram->getHistory());

	for (auto trafficLight : m_trafficLights)
		results["trafficLights"].push_back(trafficLight->getHistory());

	for (auto kvp : m_routeNodes)
		results["routeNodes"].push_back(kvp.second->getHistory());

	return results;
}
