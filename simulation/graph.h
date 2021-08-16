#include "node.h"
#include "edge.h"

#include <unordered_map>

class TrafficLight;
class PassengerNode;
class PassengerEdge;

class Graph
{
public:
	static std::pair<std::list<Edge*>, std::list<TrafficLight*>> findTramPath(Node* source, Node* target);
	static std::unordered_map<PassengerNode*, std::list<PassengerEdge*>> findPassengerPath(PassengerNode* source, PassengerNode* target);
	static float nearestTramBehindDistance(Node* node, float limit);
};
