#pragma once

#include <list>

class Edge;
class Node;
class Event;
class TrafficLight;
class Junction;
class Simulation;
class Passenger;
class PassengerNode;
class PassengerEdge;
class TramStop;

#include "include/json.hpp"
using json = nlohmann::json;

class Tram
{
public:
	Tram(int id, Edge* edge, Simulation* simulation);
	void setCurrentEdge(Edge* currentEdge);
	void setEdgesToVisit(std::list<Edge*> edgesToVisit);
	void setStopsToVisit(std::list<TramStop*> nodesToVisit);
	void setTrafficLightsToVisit(std::list<TrafficLight*> nodesToVisit);
	void setStopsTimes(std::list<float> stopsTimes);

	void setSpeed(float speed);
	void setWaitingTram(Tram* waitingTram);
	void setRoute(int route);

	int getId();
	float getPosition();
	float getMaxSpeed();
	float getLength();
	float getMaxDecelerationDistance();
	Edge* getCurrentEdge();
	std::list<TramStop*> getStopsToVisit();
	float stoppingDistance();
	Tram* getTramAhead(float limit);
	int getRoute();
	PassengerEdge* getPassengerEdge();

	enum State
	{
		resting,
		accelerating,
		decelerating,
		moving, //at constant speed
		exchangingPassangers
	};

	struct HistoryRow
	{

	};

	void setNextEvent(Event* event);
	void generateNextEvent(float time);
	inline Event* addIntermediateEvents(Event* mainEvent, float mainEventPositon, float time, Node* eventCauseNode);

	void enterNextEdge(float time);

	void beginPassengerExchange(float time);
	void updatePassengerExchange(float time);
	void requestPassengerExit(Passenger* passenger);

	void notifyTrafficLight(float time); //when traffic light notifies this
	void notifyTram(float time);		 //when tram notifies this
	void endTrip(float time);

	void updateStatistics(float time);
	void changeState(State state, float time);

	void addHistoryRow(float time);
	void addPassengerHistoryRow(float time);
	json getHistory();

	//private:
	int m_id;
	Simulation* m_simulation;
	State m_state;
	float m_position;
	float m_speed;
	Edge* m_currentEdge;
	Event* m_nextEvent;
	Junction* m_currentJunction;
	bool m_requestedGreenLight;
	Tram* m_waitingTram;
	int m_route;
	PassengerEdge* m_currentPassengerEdge;

	std::list<Edge*> m_edgesToVisit;
	std::list<TramStop*> m_stopsToVisit;
	std::list<TrafficLight*> m_trafficLightsToVisit;
	std::list<float> m_stopsTimes;

	std::list<Passenger*> m_passengers;
	std::list<Passenger*> m_exitingPassengers;

	void calculatePassengerEdge();

	float getNextStopPosition();
	float getNextTrafficLightPosition();
	float getNextTramPosition(float limit);

	void updateStatisticsAccelerating(float time);
	void updateStatisticsDecelerating(float time);
	void updateStatisticsMoving(float time);

	static constexpr float c_maxSpeed = 13.0;
	static constexpr float c_length = 50.0;
	static constexpr float c_decMaxX = 0.5 * 1.4 * 13.0 / 1.4 * 13.0 / 1.4;

	static constexpr int c_passengerCapacity = 310;
	static constexpr float c_doorOpeningTime = 3.0;
	static constexpr float c_passengerEnteringTime = 0.2;
	static constexpr float c_passengerExitingTime = 0.1;

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

	std::vector<State> m_stateHistory;
	std::vector<float> m_positionHistory;
	std::vector<float> m_timeHistory;
	std::vector<float> m_speedHistory;
	std::vector<int> m_edgeHistory;

	std::vector<float> m_timePassengerHistory;
	std::vector<int> m_passengerHistory;
};

struct TramCollisionException : public std::exception
{
	std::string m_msg;

	TramCollisionException(Tram* tram, Tram* tramAhead);

	const char* what() const throw()
	{
		return m_msg.c_str();
	}
};