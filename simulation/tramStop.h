#pragma once

#include "node.h"

class PassengerNode;
class Tram;

class TramStop : public Node
{
public:
	TramStop(int id, PassengerNode* passengerNode);

	PassengerNode* const m_passengerNode;
};