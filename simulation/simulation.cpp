#include "simulation.h"

#include <iostream>
#include "graph.h"
#include "node.h"
#include "edge.h"
//#include "event.h"

#include "tram.h"

Simulation::Simulation(json networkModel)
{
	for (json jNode : networkModel["nodes"])
	{
		unsigned long int id = jNode["id"];
		Node *node = new Node(id);

		m_nodes[id] = node;
	}

	for (json jEdge : networkModel["arrows"])
	{
		float length = jEdge["length"];
		float maxspeed = (float)jEdge["maxspeed"];
		Node *tail = m_nodes[jEdge["tail"]];
		Node *head = m_nodes[jEdge["head"]];

		Edge *edge = new Edge(tail, head, length, maxspeed);

		tail->addOutgoingEdge(edge);
		head->addIncomingEdge(edge);
	}

	std::vector<std::list<Edge *>> routesPaths;

	for (json jRoute : networkModel["routes"])
	{
		int routeId = jRoute["id"];
		std::list<unsigned long> stops = jRoute["stops"];
		std::list<Edge *> routePath;

		auto currentStop = stops.begin();
		auto nextStop = currentStop;
		nextStop++;

		while (nextStop != stops.end())
		{
			auto n1 = m_nodes[*currentStop];
			auto n2 = m_nodes[*nextStop];

			auto path = Graph::findPath(n1, n2);
			routePath.splice(routePath.end(), path);

			currentStop = nextStop;
			++nextStop;
		}
		routesPaths.push_back(routePath);
	}

	for (json jTrip : networkModel["trips"])
	{
		int routeId = jTrip["route"];

		std::list<long int> stopsIds = networkModel["routes"][routeId]["stops"];
		std::list<float> stopTimes = jTrip["times"];

		std::transform(stopTimes.begin(), stopTimes.end(), stopTimes.begin(), [](float x) -> float { return 60*x;}); //transforming minutes to seconds

		auto sIt = stopsIds.begin();
		for (auto tIt = stopTimes.begin(); tIt != stopTimes.end(); ++tIt)
		{
			if (*tIt == -1)
			{
				stopTimes.erase(tIt);
				stopsIds.erase(sIt);
				tIt--;
				sIt--;
			}
			++sIt;
		}

		std::list<Node *> tripStops;
		for (auto sId : stopsIds)
			tripStops.push_back(m_nodes[sId]);

		auto tripPath = std::list<Edge *>(routesPaths[routeId]);

		while (tripPath.front()->getTail() != tripStops.front())
			tripPath.pop_front();

		while (tripPath.back()->getHead() != tripStops.back())
			tripPath.pop_back();

		auto e = new EventTramDeploy(tripStops, stopTimes, tripPath, this);

		m_eventQueue.push(e);
	}

	std::cout << "Po\n";
}

void Simulation::run()
{
	float time = 0;
	float dt;

	while (!m_eventQueue.empty())
	{
		auto event = m_eventQueue.top();
		m_eventQueue.pop();

		dt = event->m_time - time;
		time = event->m_time;

		for(auto tram : m_trams)
			tram->updateStatistics(dt);

		std::cout<<time<<std::endl;

		event->processEvent();
		delete event;
	}
}

void Simulation::addTram(Tram *tram)
{
	m_trams.push_back(tram);
}
