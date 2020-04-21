#pragma once

#include <list>
#include <vector>
#include <queue>
#include <unordered_map>

#include "include/json.hpp"

#include "event.h"

class Node;
class Edge;
class Tram;
//class Event;
//struct CompareEventTime;

using json = nlohmann::json;

class Simulation
{
public:
	Simulation(json networkModel);
	void run();
	void addTram(Tram *tram);

private:
	std::priority_queue<Event *, std::vector<Event *>, Event::CompareTime> m_eventQueue;
	//std::priority_queue<Event *, std::vector<Event *>, CompareTime> m_eventQueue;
	//std::priority_queue<Event *, std::vector<Event *>, CompareEventTime> m_eventQueue;
	std::unordered_map<unsigned long int, Node *> m_nodes;
	std::list<Tram *> m_trams;
	//std::list<Edge> edges;
};
