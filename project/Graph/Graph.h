#pragma once
#include <vector>
#include "GraphConnection.h"
#include "GraphNode.h"

class Graph
{
public:
	Graph();

	int AddNode(const std::string& description = "Default");
	void AddConnection(int FromNodeIdx, int ToNodeIdx, float cost = 1);

	std::vector<GraphConnection> GetAllConnectionsToNode(int nodeIdx);
	std::vector<GraphConnection> GetAllConnectionsFromNode(int nodeIdx);

	GraphNode* GetNodeByIdx(int idx);

	void Reset();
private:
	std::vector<GraphNode> m_NodeVector;
	std::vector<GraphConnection> m_ConnectionVector;

	int m_NextValidNodeIdx = 0;
};

