#pragma once
class GraphConnection
{
public:
	int GetFromIdx() const { return m_From; }
	int GetToIdx() const { return m_To; }

	void SetFromIdx(int nodeIdx) { m_From = nodeIdx; }
	void SetToIdx(int nodeIdx) { m_To = nodeIdx; }

	float GetCost() const { return m_Cost; }
	void SetCost(float newCost) { m_Cost = newCost; }

private:
	int m_From;
	int m_To;

	float m_Cost;
};

