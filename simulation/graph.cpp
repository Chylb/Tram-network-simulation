#include "graph.h"

#include "tram.h"
#include "PassengerNode.h"
#include "PassengerEdge.h"

#include <queue>

std::pair<std::list<Edge*>, std::list<TrafficLight*>> Graph::findTramPath(Node* source, Node* target)
{
	typedef std::pair<Node*, float> nodeDistancePair;

	struct CompareNodeDistance
	{
		bool operator()(nodeDistancePair const& p1, nodeDistancePair const& p2) { return p1.second > p2.second; }
	};

	std::priority_queue<nodeDistancePair, std::vector<nodeDistancePair>, CompareNodeDistance> queue;
	std::unordered_map<Node*, Edge*> previous;
	std::unordered_map<Node*, float> distance;

	distance[source] = 0;

	for (auto e : source->m_outgoingEdges)
	{
		previous[e->m_head] = e;
		distance[e->m_head] = e->m_length;
		queue.push(std::make_pair(e->m_head, e->m_length));
	}

	nodeDistancePair pair = queue.top();
	queue.pop();

	while (pair.first != target)
	{
		for (auto e : pair.first->m_outgoingEdges)
		{
			if (previous[e->m_head] == nullptr)
			{
				previous[e->m_head] = e;
				distance[e->m_head] = pair.second + e->m_length;
				queue.push(std::make_pair(e->m_head, pair.second + e->m_length));
			}
		}

		pair = queue.top();
		queue.pop();
	}

	std::list<TrafficLight*> trafficLights;
	std::list<Edge*> path;

	if (target->getId() == 36)
	{
		int deb = 3;
	}

	auto edge = previous[target];
	path.push_front(edge);
	while (edge->m_tail != source)
	{
		edge = previous[edge->m_tail];
		path.push_front(edge);

		if (edge->m_head->isTrafficLight())
			trafficLights.push_front((TrafficLight*)edge->m_head);
	}

	return std::make_pair(path, trafficLights);
}

std::unordered_map<PassengerNode*, std::list<PassengerEdge*>> Graph::findPassengerPath(PassengerNode* source, PassengerNode* target)
{
	std::unordered_map<PassengerNode*, PassengerEdge*> previous;
	std::list<PassengerNode*> toVisit;

	std::unordered_map<PassengerNode*, std::list<PassengerEdge*>> result;

	for (auto e : source->m_outgoingEdges)
	{
		previous[e->m_head] = e;
		toVisit.push_back(e->m_head);
	}

	if (toVisit.size() == 0)
		return result;

	auto node = toVisit.front();
	toVisit.pop_front();

	while (node != target)
	{
		for (auto e : node->m_outgoingEdges)
		{
			if (previous[e->m_head] == nullptr)
			{
				previous[e->m_head] = e;
				toVisit.push_back(e->m_head);
			}
		}
		if (toVisit.size() == 0)
			return result;

		node = toVisit.front();
		toVisit.pop_front();
	}

	std::list<PassengerEdge*> path;

	auto edge = previous[target];
	path.push_front(edge);
	while (edge->m_tail != source)
	{
		edge = previous[edge->m_tail];
		path.push_front(edge);
	}

	for (auto e : path)
	{
		auto it = result.find(e->m_tail);
		if (it == result.end())
			result[e->m_tail] = { e };
		else
			it->second.push_back(e);
	}

	return result;
}

float Graph::nearestTramBehindDistance(Node* start, float limit)
{
	std::list<Edge*> toVisit;
	std::map<Node*, float> distance;
	distance[start] = 0.0;

	for (auto edge : start->m_incomingEdges)
		toVisit.push_back(edge);

	Edge* edge;
	while (toVisit.size() > 0)
	{
		edge = toVisit.front();
		toVisit.pop_front();

		distance[edge->m_tail] = distance[edge->m_head] + edge->m_length;

		if (edge->m_trams.size() > 0)
		{
			auto tram = edge->m_trams.front();
			return distance[edge->m_head] + edge->m_length - tram->getPosition();
		}
		else if (distance[edge->m_head] + edge->m_length < limit)
		{
			for (auto edge : edge->m_tail->m_incomingEdges)
				toVisit.push_back(edge);
		}
	}

	return INFINITY;
}
