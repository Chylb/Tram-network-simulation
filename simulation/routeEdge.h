#pragma once
#include <list>
#include <map>

class Node;
class Tram;
class RouteNode;

class RouteEdge
{
    friend class Graph;

public:
    RouteEdge(std::list<int> lines, RouteNode *tail, RouteNode *head);
    RouteNode* getHead();

private:
    std::list<int> m_lines;
    RouteNode *m_head;
    RouteNode *m_tail;
};
