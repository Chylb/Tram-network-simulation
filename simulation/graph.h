#include "node.h"
#include "edge.h"

class TrafficLight;

class Graph
{
public:
	static std::pair<std::list<Edge *>, std::list<TrafficLight *>> findPath(Node *source, Node *target);
};
