#pragma once

#include <list>

#include "node.h"

class Junction;
class Tram;

#include "include/json.hpp"
using json = nlohmann::json;

class TrafficLight : public Node
{
public:
	TrafficLight(int id);
	void setJunction(Junction* junction);

	Junction* getJunction();
	bool requestGreen(Tram* tram, float time);
	void changeState(bool state, float time);

	json getHistory();

private:
	Junction* m_junction;

	std::list<bool> m_stateHistory;
	std::list<float> m_timeHistory;
};