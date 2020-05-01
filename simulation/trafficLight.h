#pragma once

#include <list>

#include "node.h"

class TrafficLight : public Node
{
public:
	TrafficLight(int id, int phase);
	void setPhaseCount(int phaseCount);
	float timeToGreen(float time);

private:
	int m_phase;
	int m_phaseCount;
	constexpr static float c_phaseDuration = 20.0;
};