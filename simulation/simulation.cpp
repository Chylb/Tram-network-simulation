#include "simulation.h"

#include <iostream>
#include <random>

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
		std::list<int> generationDistribution = jRouteNode["generationDistribution"];
		std::list<int> absorptionRate = jRouteNode["absorptionRate"];
		int expectedGeneratedCount = jRouteNode["expectedGeneratedCount"];

		auto routeNode = new RouteNode(name, generationDistribution, absorptionRate, expectedGeneratedCount);
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
			if (m_routeNodes.find(jNode["stopName"]) == m_routeNodes.end())
			{
				node = new TramStop(id, nullptr);
			}
			else
			{
				auto routeNode = m_routeNodes[jNode["stopName"]];
				node = new TramStop(id, routeNode);
			}
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
		stopTimes.push_back(0); //last node that completes loop is't included on schedule

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

	//passenger generation

	int passengerCount = networkModel["passengerCount"];

	int sum = 0;
	int *routeNodeGenerationDistribution = new int[m_routeNodes.size()];

	//routeNode generation distribution calculation
	for (int i = 0; i < m_routeNodes.size(); i++)
	{
		auto routeNode = m_routeNodeArray[i];
		sum += routeNode->getExpectedGeneratedCount();
		routeNodeGenerationDistribution[i] = sum;
	}

	int **routeNodeAbsorptionDistribution = new int *[24];
	std::uniform_int_distribution<> *absorption_dist = new std::uniform_int_distribution<>[24];

	//routeNode absorption distribution calculation
	for (int h = 0; h < 24; h++)
	{
		routeNodeAbsorptionDistribution[h] = new int[m_routeNodes.size()];
		sum = 0;
		for (int i = 0; i < m_routeNodes.size(); i++)
		{
			auto routeNode = m_routeNodeArray[i];
			sum += routeNode->getAbsorptionRate(h);
			routeNodeAbsorptionDistribution[h][i] = sum;
		}
		absorption_dist[h] = std::uniform_int_distribution<>(0, sum);
	}

	std::minstd_rand0 rng;
	std::uniform_int_distribution<> passenger_dist(0, passengerCount);
	std::uniform_real_distribution<> real_dist(0, 1);

	//finding paths
	std::unordered_map<RouteNode *, std::list<int>> ***paths = new std::unordered_map<RouteNode *, std::list<int>> **[m_routeNodes.size()];

	for (int i = 0; i < m_routeNodes.size(); ++i)
	{
		paths[i] = new std::unordered_map<RouteNode *, std::list<int>> *[m_routeNodes.size()];

		for (int j = 0; j < m_routeNodes.size(); ++j)
		{
			auto node1 = m_routeNodeArray[i];
			auto node2 = m_routeNodeArray[j];

			auto path = Graph::findPassengerPath(node1, node2);
			paths[i][j] = new std::unordered_map<RouteNode *, std::list<int>>(path);
		}
	}

	//generating passengers
	for (int i = 0; i < passengerCount; ++i)
	{
		int r = passenger_dist(rng);
		int ix1 = 0;
		while (routeNodeGenerationDistribution[ix1] < r)
			++ix1;
		auto node1 = m_routeNodeArray[ix1];

		int h = node1->randomPassengerSpawnHour(&rng);

		int ix2;
		do
		{
			r = absorption_dist[h](rng);
			ix2 = 0;
			while (routeNodeAbsorptionDistribution[h][ix2] < r)
				++ix2;
		} while (ix1 == ix2);
		auto node2 = m_routeNodeArray[ix2];

		float time = 3600 * ((float)h + real_dist(rng));

		auto path = paths[ix1][ix2];

		auto event = new EventSpawnPassenger(time, node1, node2, path);
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
