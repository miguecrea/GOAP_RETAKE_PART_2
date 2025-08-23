#include"Planner.h"
#include"../Actions/Actions.h"
#include"../WorldStates/WorldStates.h"
#include"../../Graph/Dijkstra.h"


Planner::Planner(std::vector<BaseWorldState*>* WorldStates):
	m_pWorldStates{ WorldStates }, m_pGraph{ std::make_unique<Graph>()}
{

    InitializeGoalsAndActions();
    CreateGraph();
}

void Planner::InitializeGoalsAndActions()
{
    m_Actions =
    {
        new Wander(),
        new ConsumeSavedMedKit(),
        new ConsumeSavedFood(),
        new GoToNearestSeenGun(),
        new GoToNearestSeenMedKit(),
        new GoToNearestSeenFood(),
        new LeaveHouse(),
        new MoveIntoHouse(),
        new PickupMedKit(),
        new PickupWeapon(),
        new PickupFood(),
        new RunFromPurge(),
        new ShootEnemyInView(),
        new SprintAwayFromEnemy(),
        new TurnReallyFast()
    };


    m_Goals =
    {
        new IsInPurgeZoneState(false),
        new RecentlyBittenState(false),
        new ZombieInViewState(false),
        new IsHurtState(false),
        new HasWeaponState(true),
        new IsHungry(false),
        new HasSavedWeaponsWithAcceptableAmmo(true),
        new IsLoadedWithMedKits(true),
        new HasSavedUpFood(true),
        new HasVisitedAllSeenHouses(true)
    };
}

Planner::~Planner()
{
}

bool Planner::CalculateAction(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
    BaseWorldState * currentGoal = nullptr;

    CurrentActionInfo currentActionInfo{};

    for (auto * goal : m_Goals) 
    {
        for (auto* state : * m_pWorldStates)
        {
            if (state->m_Name != goal->m_Name) continue;

            if (state->m_Predicate == goal->m_Predicate)
            {
                if (m_AccomplishedGoals.insert(goal).second)
                    std::cout << "Accomplished Goal: " << goal->m_Name << '\n';
            }
            else
            {

                m_AccomplishedGoals.erase(goal);
                currentActionInfo = ChooseCurrentAction(goal);

                if (currentActionInfo.IsValid())
                {
                    currentGoal = goal;

                    if (m_CurrentGoal != goal->m_Name)
                    {
                        m_CurrentGoal = goal->m_Name;

                        std::cout << "\n\n Current Goal: " << goal->m_Name << '\n' << "Action Plan:\n";

                        for (size_t i = 0; i < currentActionInfo.PathInfo->Path.size(); ++i)
                        {
                            std::cout << currentActionInfo.PathInfo->Path[i]->GetDescription() << '\n';

                        }
                        std::cout << "\n\n";
                    }
                    break; 
                }
                
            }
        }
        if (currentGoal) break;
    }

    if (!currentGoal && !m_CurrentGoal.empty())
    {
        m_CurrentGoal.clear();
        std::cout << "No Goal Found\n";
    }

    BaseAction * currentAction = nullptr;


    if (currentActionInfo.IsValid() &&
        currentGoal)
    {
        const std::string& actionDesc = currentActionInfo.PathInfo->Path.front()->GetDescription();


        if (m_CurrentAction != actionDesc)
        {
            m_CurrentAction = actionDesc;
            std::cout << "Current Action: " << actionDesc << '\n';
        }

        currentAction = currentActionInfo.CurrentAction;
        m_HadActionLastTick = true;
    }
    else
    {
        if (m_HadActionLastTick)
        {
            m_HadActionLastTick = false;
            std::cout << "No More Actions\n";
        }
        currentAction = m_Actions.front(); 
    }

    return currentAction->Execute(elapsedSec, steeringOutput, iFace);

}

void Planner::CreateGraph()
{ 
    for (BaseAction* action : m_Actions)
    {
        action->SetGraphNodeIndex(m_pGraph->AddNode());
        m_pGraph->GetNodeByIdx(action->GetGraphNodeIndex())->SetDescription(action->GetName());
        m_pGraph->GetNodeByIdx(action->GetGraphNodeIndex())->Weight = action->GetWeight();
    }

    for (BaseAction* action : m_Actions)
    {

        for (BaseWorldState* actionPreCondition : action->GetPreconditions())
        {
            for (BaseAction* otherAction : m_Actions) //loop one action with otehr action
            {
                if (otherAction == action) continue;

                for (auto* otherActionEffectOnWorld : otherAction->GetEffects())
                {
                    if (otherActionEffectOnWorld->m_Name == actionPreCondition->m_Name &&
                        otherActionEffectOnWorld->m_Predicate == actionPreCondition->m_Predicate)
                    {
                        m_pGraph->AddConnection(otherAction->GetGraphNodeIndex(), action->GetGraphNodeIndex(), otherAction->GetWeight());
                        break;
                    }
                }
            }
        }
    }

    std::string currentKey{};
    float currentValue{};
    PathInfo currentPathInfo{};

    for (size_t idx = 0; idx < m_Actions.size(); idx++)
    {
        for (size_t otherIdx = 0; otherIdx < m_Actions.size(); otherIdx++)
        {
            currentKey = m_Actions[idx]->GetName() + m_Actions[otherIdx]->GetName();
            currentPathInfo.Path = Dijkstra::FindPath(m_pGraph.get(), m_pGraph->GetNodeByIdx(idx), m_pGraph->GetNodeByIdx(otherIdx));

            if (!currentPathInfo.Path.empty() && // path is not empty 
                std::find(
                    currentPathInfo.Path.begin(),
                    currentPathInfo.Path.end(), 
                    m_pGraph->GetNodeByIdx(otherIdx))
                != currentPathInfo.Path.end()) // current path contains end node
            {
                currentValue = 0;
                for (size_t i = 0; i < currentPathInfo.Path.size(); i++)
                {
                    currentValue += currentPathInfo.Path[i]->Weight;
                }

                currentPathInfo.TotalPathCost = currentValue;

                m_StartEndNodeString_To_Path[currentKey] = currentPathInfo;
            }
        }
    }

    
}

Planner::CurrentActionInfo Planner::ChooseCurrentAction(BaseWorldState * stateToAchieve)
{
    std::vector<BaseAction *> startNodes{};
    std::vector<BaseAction *> endNodes{};


    for (BaseAction * action : m_Actions)
    {
        bool conditionsMet = std::all_of(action->GetPreconditions().begin(), action->GetPreconditions().end(), [this](BaseWorldState  * pre)
            {
                for (auto* world : *m_pWorldStates)
                {
                    if (pre->m_Name == world->m_Name && pre->m_Predicate != world->m_Predicate)
                    {
                        return false;
                    }

                }

                return true;
            }

        );

        if (conditionsMet)
            startNodes.push_back(action); 

        bool achievesGoal = std::any_of(action->GetEffects().begin(), action->GetEffects().end(), [stateToAchieve](BaseWorldState * eff)
            {
                return eff->m_Name == stateToAchieve->m_Name && eff->m_Predicate == stateToAchieve->m_Predicate;
            }
        );

        if (achievesGoal) 
            endNodes.push_back(action);

    }
 
    std::string currentKey{};
    CurrentActionInfo currentActionInfo{};
    float currentHighestValue = INFINITY;
    
    for (size_t startNodeIdx = 0; startNodeIdx < startNodes.size(); startNodeIdx++)
    {
        for (size_t endNodeIdx = 0; endNodeIdx < endNodes.size(); endNodeIdx++)
        {
            currentKey = startNodes[startNodeIdx]->GetName() + endNodes[endNodeIdx]->GetName();

            auto& currentPair = m_StartEndNodeString_To_Path.find(currentKey);

            if (currentPair != m_StartEndNodeString_To_Path.end()) // contains
            {
                if (currentPair->second.TotalPathCost < currentHighestValue)
                {
                    currentHighestValue = currentPair->second.TotalPathCost;
                    currentActionInfo.CurrentAction = startNodes[startNodeIdx];
                    currentActionInfo.PathInfo = &currentPair->second;
                }
            }
        }
    }

    return currentActionInfo;
}
