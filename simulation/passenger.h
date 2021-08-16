#pragma once

#include <unordered_map>
#include <list>

#include "include/json.hpp"
using json = nlohmann::json;

class PassengerNode;
class PassengerEdge;
class Tram;
class TramStop;

class Passenger
{
public:
	Passenger(float time, PassengerNode* startNode, PassengerNode* endNode, std::unordered_map<PassengerNode*, std::list<PassengerEdge*>>* path);

	void notifyInside(float time, TramStop* tramStop);

	void enterTram(float time, Tram* tram);
	void exitTram(float time, PassengerNode* routeNode);

	json getHistory();

private:
	void addHistoryRow(float time);

	PassengerNode* m_currentNode;
	Tram* m_currentTram;
	PassengerNode* m_endNode;
	std::unordered_map<PassengerNode*, std::list<PassengerEdge*>>* m_path;

	std::list<float> m_timeHistory;
	std::list<Tram*> m_tramHistory;
	std::list<PassengerNode*> m_nodeHistory;
};