#pragma once

#include <list>

class Edge;
class Node;
class Event;
class TrafficLight;

#include "include/json.hpp"
using json = nlohmann::json;

class Tram
{
public:
	Tram(int id, Edge *edge);
	void setCurrentEdge(Edge *currentEdge);
	void setEdgesToVisit(std::list<Edge *> edgesToVisit);
	void setStopsToVisit(std::list<Node *> nodesToVisit);
	void setTrafficLightsToVisit(std::list<TrafficLight *> nodesToVisit);
	void setStopsTimes(std::list<float> stopsTimes);

	void setSpeed(float speed);

	int getId();
	float getPosition();
	float getMaxSpeed();
	Edge *getCurrentEdge();
	std::list<Node *> getStopsToVisit();
	float stoppingDistance();
	Tram* getTramAhead(float limit);

	enum State
	{
		resting,
		accelerating,
		decelerating,
		moving, //at constant speed
		exchangingPassangers
	};

	Event *getNextEvent(float time);
	Event *getEventAfterDeceleration(float time, float *timeToNextEvent, Node *eventCauseNode);
	Event *addIntermediateEvents(Event *mainEvent, float mainEventPositon, float time, Node *eventCauseNode);

	void enterNextEdge(float time);
	float exchangePassengers(float time);
	float waitAtTrafficLights(float time);

	void updateStatistics(float time);
	void changeState(State state, float time);

	void addHistoryRow(float time);
	json getHistory();

private:
	int m_id;
	State m_state;
	float m_position;
	float m_speed;
	Edge *m_currentEdge;

	std::list<Edge *> m_edgesToVisit;
	std::list<Node *> m_stopsToVisit;
	std::list<TrafficLight *> m_trafficLightsToVisit;
	std::list<float> m_stopsTimes;

	float getNextStopPosition();
	float getNextTrafficLightPosition();
	float getNextTramPosition(float limit);

	void updateStatisticsAccelerating(float time);
	void updateStatisticsDecelerating(float time);
	void updateStatisticsMoving(float time);

	static constexpr float c_maxSpeed = 13.0;
	static constexpr float c_length = 25.0;
	static constexpr float c_decMaxX = 0.5 * 1.4 * 13.0 / 1.4 * 13.0 / 1.4;

	//acceleration specifications
	static float acc_v_of_t(float t, float v0);
	static float acc_t_of_v(float v, float v0);
	static float acc_x_of_t(float t, float v0);
	static float acc_t_of_x(float x, float v0);

	//deceleration specifications
	static float dec_v_of_t(float t, float v0);
	static float dec_t_of_v(float v, float v0);
	static float dec_x_of_t(float t, float v0);
	static float dec_t_of_x(float x, float v0);

	static float accdec_t1_of_x(float x, float v0, float v2);

	float m_x0;
	float m_t0;
	float m_v0;

	std::list<State> m_stateHistory;
	std::list<float> m_positionHistory;
	std::list<float> m_timeHistory;
	std::list<float> m_speedHistory;
	std::list<int> m_edgeHistory;
};

struct TramCollisionException : public std::exception
{
	std::string m_msg;

	TramCollisionException(Tram *tram, Tram* tramAhead);

	const char *what() const throw()
	{
		return m_msg.c_str();
	}
};