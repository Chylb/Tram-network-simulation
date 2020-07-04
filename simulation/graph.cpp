#include "graph.h"

#include "tram.h"
#include "routeNode.h"
#include "routeEdge.h"

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

std::unordered_map<RouteNode *, std::list<int>> Graph::findPassengerPath(RouteNode *source, RouteNode *target)
{
    std::unordered_map<RouteNode *, RouteEdge *> previous;
    std::list<RouteNode *> toVisit;

    for (auto e : source->m_outgoingEdges)
    {
        previous[e->m_head] = e;
        toVisit.push_back(e->m_head);
    }

    if (toVisit.size() == 0)
    {
        std::unordered_map<RouteNode *, std::list<int>> res;
        return res;
    }

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
        {
            std::unordered_map<RouteNode *, std::list<int>> res;
            return res;
        }
        node = toVisit.front();
        toVisit.pop_front();
    }

    std::list<RouteEdge *> path;

    auto edge = previous[target];
    path.push_front(edge);
    while (edge->m_tail != source)
    {
        edge = previous[edge->m_tail];
        path.push_front(edge);
    }

    std::unordered_map<RouteNode *, std::list<int>> result;
    for (auto e : path)
    {
        result[e->m_tail] = e->m_lines;
    }

    return result;
}

float Graph::nearestTramBehindDistance(Node* start, float limit) {
	std::list<Edge*> toVisit;
	std::map<Node*, float> distance;
	distance[start] = 0.0;

	for (auto edge : start->m_incomingEdges)
		toVisit.push_back(edge);

	Edge* edge;
	while (toVisit.size() > 0) {
		edge = toVisit.front();
		toVisit.pop_front();

		distance[edge->m_tail] = distance[edge->m_head] + edge->m_length;

		if (edge->m_trams.size() > 0) {
			auto tram = edge->m_trams.front();
			return distance[edge->m_head] + edge->m_length - tram->getPosition();
		}
		else if (distance[edge->m_head] + edge->m_length < limit) {
			for (auto edge : edge->m_tail->m_incomingEdges) 
				toVisit.push_back(edge);
		}
	}

	return INFINITY;
}
