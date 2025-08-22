
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include "Dijkstra.h"


std::vector<GraphNode*> Dijkstra::FindPath(Graph* pGraph, GraphNode* pStartNode, GraphNode* pGoalNode)
{
	std::vector<GraphNode*> path;

	std::priority_queue<
		std::shared_ptr<NodeRecord>,
		std::vector<std::shared_ptr<NodeRecord>>,
		CompareCost
	> openList;

	std::unordered_map<GraphNode*, std::shared_ptr<NodeRecord>> visited;

	auto startRecord = std::make_shared<NodeRecord>(pStartNode, 0.0f, nullptr);
	openList.push(startRecord);
	visited[pStartNode] = startRecord;

	std::shared_ptr<NodeRecord> goalRecord = nullptr;

	while (!openList.empty())
	{
		auto current = openList.top();
		openList.pop();

		if (current->pNode == pGoalNode)
		{
			goalRecord = current;
			break;
		}

		auto connections = pGraph->GetAllConnectionsFromNode(current->pNode->GetIndex());

		for (const auto& conn : connections)
		{
			GraphNode* toNode = pGraph->GetNodeByIdx(conn.GetToIdx());
			float newCost = current->costSoFar + conn.GetCost();

			auto it = visited.find(toNode);
			if (it == visited.end() || newCost < it->second->costSoFar)
			{
				auto nextRecord = std::make_shared<NodeRecord>(toNode, newCost, current);
				openList.push(nextRecord);
				visited[toNode] = nextRecord;
			}
		}
	}

	if (goalRecord)
	{
		NodeRecord* currentNodeRecord{ goalRecord.get() };

		while (currentNodeRecord)
		{
			path.push_back(currentNodeRecord->pNode);
			currentNodeRecord = currentNodeRecord->previousRecord.get();
		}
		std::reverse(path.begin(), path.end());
	}

	return path;
}
