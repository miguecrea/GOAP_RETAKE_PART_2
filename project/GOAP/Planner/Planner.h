

#pragma once

#include "../../stdafx.h"

#include"../WorldStates/BaseWorldState.h"
#include"../Actions/BaseAction.h"
#include "Exam_HelperStructs.h"
#include <vector>
#include <string>

#include "../../Graph/Graph.h"
#include "unordered_set"

class IExamInterface;


class Planner
{
public:
	Planner(const std::vector<std::unique_ptr<BaseWorldState>>& WorldStates);
	~Planner();
	Planner(const Planner&) = delete;
	Planner(Planner&&) noexcept = delete;
	Planner& operator=(const Planner&) = delete;
	Planner& operator=(Planner&&) noexcept = delete;
	Planner(Planner& other) = delete;

	bool CalculateAction(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace);

private:

	void MakeGraph(BaseWorldState* stateToAchieve);

	std::vector<std::unique_ptr<BaseWorldState>> m_pWorldStates;


	std::vector<std::unique_ptr<BaseAction>> m_Actions;
	std::vector<std::unique_ptr<BaseWorldState>> m_Goals;

	std::unique_ptr<Graph> m_pGraph{};



	std::string m_CurrentGoal;
	std::string m_CurrentAction;
	bool m_HadActionLastTick;
	std::unordered_set<BaseWorldState*> m_AccomplishedGoals;

};
