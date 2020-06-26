#pragma once

#include <list>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "include/json.hpp"

#include "event.h"

class Node;
class Edge;
class Tram;

using json = nlohmann::json;

class Simulation
{
public:
	Simulation(json networkModel);
	void run();
	void addTram(Tram *tram);
	void removeTram(Tram *tram);
	void addEvent(Event *event);
	void removeEvent(Event *event);
	json getResults();

private:
	std::priority_queue<Event *, std::vector<Event *>, Event::CompareTime> m_eventQueue;
	std::unordered_set<Event *> m_removedEvents;

	std::unordered_map<int, Node *> m_nodes;
	std::list<Tram *> m_trams;
	std::list<Tram *> m_removedTrams;
	std::list<TrafficLight *> m_trafficLights;
};
