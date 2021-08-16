#include "tramStop.h"

#include "PassengerNode.h"

TramStop::TramStop(int id, PassengerNode* passengerNode) : Node(id, false, false, true), m_passengerNode(passengerNode)
{
}