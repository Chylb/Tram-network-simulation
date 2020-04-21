#pragma once

#include <list>

class Edge;
class Node;

class Tram
{
public:
	Tram();
	void setCurrentEdge(Edge *currentEdge);
	void setEdgesToVisit(std::list<Edge *> edgesToVisit);
	void setStopsToVisit(std::list<Node *> nodesToVisit);
	void setStopsTimes(std::list<float> stopsTimes);

	enum State
	{
		resting,
		accelerating,
		decelerating,
		moving,
		exchangingPassangers
	};

	void updateStatistics(float dt);

private:
	int m_state;
	float m_velocity;
	float m_distance;
	Edge *m_currentEdge;

	std::list<Edge *> m_edgesToVisit;
	std::list<Node *> m_stopsToVisit;
	std::list<float> m_stopsTimes;

	void updateStatisticsAccelerating(float dt);
	void updateStatisticsDecelerating(float dt);
	void updateStatisticsMoving(float dt);
};
