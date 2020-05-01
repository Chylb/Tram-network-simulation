#include "trafficLight.h"

#include <math.h>

TrafficLight::TrafficLight(int id, int phase) : Node(id, true, false)
{
    m_phase = phase;
}

void TrafficLight::setPhaseCount(int phaseCount)
{
    m_phaseCount = phaseCount;
}

float TrafficLight::timeToGreen(float time)
{
    float cycleTime = fmod(time, c_phaseDuration * m_phaseCount);
    float timeToGreen = m_phase * c_phaseDuration - cycleTime;
    if (timeToGreen > 0.0)
        return timeToGreen;
    else if (timeToGreen > -c_phaseDuration)
        return 0.0;

    return timeToGreen + c_phaseDuration * m_phaseCount;
}
