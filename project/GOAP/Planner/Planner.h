

#pragma once

#include "../../stdafx.h"

#include"../WorldStates/BaseWorldState.h"
#include"../Actions/BaseAction.h"
#include "Exam_HelperStructs.h"
#include <vector>
#include <string>

#include "../../Graph/Graph.h"
#include "unordered_set"
#include "unordered_map"

class IExamInterface;
class WorldStates;


class Planner
{
public:
	Planner(std::vector<BaseWorldState*>* WorldStates);
	void InitializeGoalsAndActions();
	~Planner();
	Planner(const Planner&) = delete;
	Planner(Planner&&) noexcept = delete;
	Planner& operator=(const Planner&) = delete;
	Planner& operator=(Planner&&) noexcept = delete;
	Planner(Planner& other) = delete;

	bool CalculateAction(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace);

private:

	struct PathInfo
	{
		float TotalPathCost{};
		std::vector<GraphNode*> Path;

	};

	struct CurrentActionInfo 
	{

		BaseAction * CurrentAction{ nullptr };
		PathInfo * PathInfo{ nullptr };

		bool IsValid()const 
		{
			return CurrentAction && PathInfo && !PathInfo->Path.empty();
		}
	};

	void CreateGraph();

	CurrentActionInfo ChooseCurrentAction(BaseWorldState * stateToAchiev);



	std::vector<BaseWorldState*> * m_pWorldStates;

	std::vector<BaseAction *> m_Actions;
	std::vector<BaseWorldState*> m_Goals;

	std::unique_ptr<Graph> m_pGraph{};

	
	//Just for debug purposes --> all I really need is total path cost

	std::unordered_map<std::string, PathInfo> m_StartEndNodeString_To_Path;

	

	std::string m_CurrentGoal;
	std::string m_CurrentAction;
	bool m_HadActionLastTick;
	std::unordered_set<BaseWorldState*> m_AccomplishedGoals;

};
