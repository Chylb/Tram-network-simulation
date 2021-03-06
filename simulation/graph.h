#include "node.h"
#include "edge.h"

#include <unordered_map>

class TrafficLight;
class RouteNode;
class RouteEdge;

class Graph
{
public:
	static std::pair<std::list<Edge *>, std::list<TrafficLight *>> findTramPath(Node *source, Node *target);
	static std::unordered_map<RouteNode *, std::list<RouteEdge *>> findPassengerPath(RouteNode *source, RouteNode *target);
	static float nearestTramBehindDistance(Node *node, float limit);
};
