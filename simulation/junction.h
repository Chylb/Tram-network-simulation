#pragma once

#include <list>

class TrafficLight;
class Node;
class Tram;

typedef std::pair<Tram*, TrafficLight*> request;

class Junction
{
public:
	Junction();
	void addTrafficLight(TrafficLight* trafficLight);
	void addJunctionExit(Node* exit);

	bool requestGreen(Tram* tram, TrafficLight* TrafficLight, float time);
	void removeTram(Tram* tram, float time);

private:
	std::list<TrafficLight*> m_trafficLights;
	std::list<Node*> m_exits;
	std::list<request> m_requests;
	std::list<Tram*> m_tramsOnJunction;
	TrafficLight* m_greenLight;
};