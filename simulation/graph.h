#include "node.h"
#include "edge.h"

class Graph
{
public:
	static std::list<Edge *> findPath(Node *source, Node *target);
};
