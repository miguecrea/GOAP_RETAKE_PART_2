#pragma once
#include <vector>
#include <memory>
#include "Graph.h"

struct NodeRecord
{
	GraphNode* pNode = nullptr;
	float costSoFar = 0.0f;
	std::shared_ptr<NodeRecord> previousRecord;

	NodeRecord(GraphNode* node, float cost, std::shared_ptr<NodeRecord> prev)
		: pNode(node), costSoFar(cost), previousRecord(prev) {
	}
};

// Comparison for the priority queue (min-heap)
struct CompareCost
{
	bool operator()(const std::shared_ptr<NodeRecord>& lhs, const std::shared_ptr<NodeRecord>& rhs) const
	{
		return lhs->costSoFar > rhs->costSoFar;
	}
};

class Dijkstra
{
public:
	static std::vector<GraphNode*> FindPath(Graph* pGraph, GraphNode* pStartNode, GraphNode* pGoalNode);
	
};

