#include"Planner.h"
#include"../Actions/Actions.h"
#include"../WorldStates/WorldStates.h"

Planner::Planner(const std::vector<std::unique_ptr<BaseWorldState>> & pWorldStates) :
	m_pWorldStates{ std::move(m_pWorldStates) }, m_pGraph{ std::make_unique<Graph>()}
{


	m_Actions.push_back(std::make_unique<Wander>());
	m_Actions.push_back(std::make_unique<ConsumeSavedFood>());
	m_Actions.push_back(std::make_unique<ConsumeSavedMedKit>());
	m_Actions.push_back(std::make_unique<GoToNearestSeenItem>(eItemType::FOOD));
	m_Actions.push_back(std::make_unique<GoToNearestSeenItem>(eItemType::MEDKIT));
	m_Actions.push_back(std::make_unique<GoToNearestSeenItem>(eItemType::SHOTGUN));
	m_Actions.push_back(std::make_unique<GoToNearestSeenItem>(eItemType::PISTOL));
	m_Actions.push_back(std::make_unique<LeaveHouse>());
	m_Actions.push_back(std::make_unique<MoveIntoHouse>());
	m_Actions.push_back(std::make_unique<PickUpItem>(eItemType::MEDKIT));
	m_Actions.push_back(std::make_unique<PickUpItem>(eItemType::FOOD));
	m_Actions.push_back(std::make_unique<PickUpItem>(eItemType::PISTOL));
	m_Actions.push_back(std::make_unique<PickUpItem>(eItemType::SHOTGUN));
	m_Actions.push_back(std::make_unique<RunFromPurge>());
	m_Actions.push_back(std::make_unique<ShootEnemy>());
	m_Actions.push_back(std::make_unique<EvadeEnemy>());



	m_Goals.push_back(std::make_unique<IsInPurgeZoneState>(false));
	m_Goals.push_back(std::make_unique<ZombieInViewState>(false));
	m_Goals.push_back(std::make_unique<HasWeaponState>(true));
	m_Goals.push_back(std::make_unique<IsHungry>(false));
	m_Goals.push_back(std::make_unique<HasSavedUpItem>(true, eItemType::FOOD));
	m_Goals.push_back(std::make_unique<IsLoadedWithMedKits>(true));
	m_Goals.push_back(std::make_unique<HasVisitedAllSeenHouses>(true));
	m_Goals.push_back(std::make_unique<HasSavedWeaponsWithAcceptableAmmo>(true));
	m_Goals.push_back(std::make_unique<SafeFromEnemy>(true));
	m_Goals.push_back(std::make_unique<IsInjured>(false));
	m_Goals.push_back(std::make_unique<IsLowOnAmmo>(false));




}

Planner::~Planner()
{
}

bool Planner::CalculateAction(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	return m_Actions[0]->Execute(elapsedSec, steeringOutput, iFace);

}

void Planner::MakeGraph(BaseWorldState * stateToAchieve)
{


    m_pGraph->Reset();

    int startNode = m_pGraph->AddNode("startNode");
    int endNode = m_pGraph->AddNode("endNode");


    for (auto & action : m_Actions)
    {
        action->SetGraphNodeIndex(m_pGraph->AddNode());

        m_pGraph->GetNodeByIdx(action->GetGraphNodeIndex())->SetDescription(action->GetName());

        bool conditionsMet = std::all_of(action->GetPreconditions().begin(), action->GetPreconditions().end(), [this](auto & pre)
            
                for (auto & world : m_pWorldStates)
                {
                    if (pre->m_Name == world->m_Name && pre->Predicate != world->Predicate)
                    {
                        return false;
                    }

                }

                return true; 
        );

        if (conditionsMet)  
            m_pGraph->AddConnection(startNode, action->m_GraphNodeIdx, 0); 




        bool achievesGoal = std::any_of(action->GetEffectsOnWorld().begin(), action->GetEffectsOnWorld().end(), [goalState](WorldState* eff)
            {
                return eff->m_Name == goalState->m_Name && eff->Predicate == goalState->Predicate;
            }
        );


        if (achievesGoal)                 
            m_pGraph->AddConnection(action->m_GraphNodeIdx, endNode, 0);

    }

    for (Action * action : m_Actions)
    {

        for (WorldState* actionPreCondition : action->GetPreconditions())
        {
            for (Action* otherAction : m_Actions) 
            {
                if (otherAction == action) continue;

                for (auto* otherActionEffectOnWorld : otherAction->GetEffectsOnWorld())
                {
                    if (otherActionEffectOnWorld->m_Name == actionPreCondition->m_Name &&
                        otherActionEffectOnWorld->Predicate == actionPreCondition->Predicate)
                    {
                        m_pGraph->AddConnection(otherAction->m_GraphNodeIdx, action->m_GraphNodeIdx, otherAction->GetWeight());
                        break;
                    }
                }
            }
        }
    }
}
