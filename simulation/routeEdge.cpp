#include "routeEdge.h"

class RouteNode;

RouteEdge::RouteEdge(std::list<int> lines, RouteNode *tail, RouteNode *head){
    m_lines = lines;
    m_tail = tail;
    m_head = head;
}