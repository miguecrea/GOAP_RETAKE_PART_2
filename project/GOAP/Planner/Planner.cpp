#include"Planner.h"
#include"../Actions/Actions.h"
#include"../WorldStates/WorldStates.h"
#include"../../Graph/Dijkstra.h"


Planner::Planner(std::vector<BaseWorldState*>* WorldStates):
	m_pWorldStates{ WorldStates }, m_pGraph{ std::make_unique<Graph>()}
{

    m_Actions =
    {
        new Wander(),
        new ConsumeSavedMedKit(),
        new ConsumeSavedFood(),
        new GoToNearestSeenGun(),
        new GoToNearestSeenMedKit(), //can have a parameter .parameter 
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

    //sort them as  apiority

    m_Goals =
    {
        //this is how we wan to set alol the goals  not hungry has sabed up food etc 
       new IsInPurgeZoneState(false),
       new RecentlyBittenState(false),
       new ZombieInViewState(false),
       new IsHurtState(false),
       new HasWeaponState(true),
       new IsHungry(false),
       new HasSavedWeaponsWithAcceptableAmmo(true), //
       new IsLoadedWithMedKits(true), //
       new HasSavedUpFood(true),
       new HasVisitedAllSeenHouses(true)
    };

    //


}

Planner::~Planner()
{
}

bool Planner::CalculateAction(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	

    BaseWorldState * currentGoal = nullptr;

    std::vector<GraphNode*> actions;



    for (auto * goal : m_Goals)  //my gols 
    {
        for (auto* state : * m_pWorldStates)  ///how it is in the world rightm 
        {
            if (state->m_Name != goal->m_Name) continue;

            if (state->m_Predicate == goal->m_Predicate)
            {
                if (m_AccomplishedGoals.insert(goal).second)
                    std::cout << "Accomplished Goal: " << goal->m_Name << '\n';
            }
            else
            {
                //erase it form there 

                m_AccomplishedGoals.erase(goal);
                //
                MakeGraph(goal);


                //create s aseries fo graph nodes 

                //check is goal is achibvable 
                actions = Dijkstra::FindPath(m_pGraph.get(), m_pGraph->GetNodeByIdx(0), m_pGraph->GetNodeByIdx(1));


                if (std::find(actions.begin(), actions.end(), m_pGraph->GetNodeByIdx(1)) != actions.end())
                {
                    currentGoal = goal;


                    if (m_CurrentGoal != goal->m_Name)
                    {
                        m_CurrentGoal = goal->m_Name;

                        std::cout << "\n\n Current Goal: " << goal->m_Name << '\n' << "Action Plan:\n";

                        for (size_t i = 1; i + 1 < actions.size(); ++i)
                        {
                            std::cout << actions[i]->GetDescription() << '\n';

                        }

                        std::cout << "\n\n";
                    }
                    break;  //try again until find a goal that can be acomplished 
                    //oh we found a goal that can be accomplished 
                }
            }
        }
        if (currentGoal) break;
    }




    //no 

    if (!currentGoal && !m_CurrentGoal.empty())
    {
        m_CurrentGoal.clear();
        std::cout << "No Goal Found\n";
    }



    BaseAction * currentAction = nullptr;

    //accomplsih the current goal we got the Actions saved 

    if (!actions.empty() && currentGoal)
    {
        const std::string& actionDesc = actions[1]->GetDescription();


        if (m_CurrentAction != actionDesc)
        {
            m_CurrentAction = actionDesc;
            std::cout << "Current Action: " << actionDesc << '\n';
        }


        //actions  that can be doine 

        //looks for that one action
        for (auto* action : m_Actions)
        {
            if (action->GetName() == actionDesc)
            {
                currentAction = action;
                break;
            }
        }

        m_HadActionLastTick = true;
    }
    else
    {
        if (m_HadActionLastTick)
        {
            m_HadActionLastTick = false;
            std::cout << "No More Actions\n";
        }
        currentAction = m_Actions.front(); // default: Wander
    }

    return currentAction->Execute(elapsedSec, steeringOutput, iFace);

}

void Planner::MakeGraph(BaseWorldState * stateToAchieve)
{

    //reset the gragh
    m_pGraph->Reset();

    int startNode = m_pGraph->AddNode("startNode");
    int endNode = m_pGraph->AddNode("endNode");

    //  All possible Actions we can take 
    //loop over that

    for (BaseAction * action : m_Actions)
    {
        action->SetGraphNodeIndex(m_pGraph->AddNode());



        //set the name 
        m_pGraph->GetNodeByIdx(action->GetGraphNodeIndex())->SetDescription(action->GetName());
        bool conditionsMet = std::all_of(action->GetPreconditions().begin(), action->GetPreconditions().end(), [this](BaseWorldState  * pre)
            {

                //check ecvey world state  


                for (auto* world : *m_pWorldStates)
                {
                    //check precondiiton is on the same state as it is in the world state 
                    if (pre->m_Name == world->m_Name && pre->m_Predicate != world->m_Predicate)
                    {
                        return false;
                    }

                }

                return true;
            }

            //if preconditions 






        );


        //if preconditionsMet is true, we can use this action immediately, so we connect it from the startNode:

        if (conditionsMet)  // cost is still 0 
            m_pGraph->AddConnection(startNode, action->GetGraphNodeIndex(), 0); //add connection

        // Check if action achieves goal   //goal is on top 

        //effect on world is same as world state //check isf any of it os effects 


        bool achievesGoal = std::any_of(action->GetEffects().begin(), action->GetEffects().end(), [stateToAchieve](BaseWorldState * eff)
            {
                return eff->m_Name == stateToAchieve->m_Name && eff->m_Predicate == stateToAchieve->m_Predicate;
            }
        );


        //dont a I have to meet the condition if 
        if (achievesGoal)                 //
            m_pGraph->AddConnection(action->GetGraphNodeIndex(), endNode, 0);



    }



    // If Action A produces an effect that satisfies a precondition for Action B, then Action B can logically follow Action A.

   // Add a connection from A to B.
    //this is done for every action 

    // Interconnect actions by effects <-> preconditions
    for (BaseAction  * action : m_Actions)
    {

        //[reconditions 
        for (BaseWorldState * actionPreCondition : action->GetPreconditions())
        {
            for (BaseAction * otherAction : m_Actions) //loop one action with otehr action
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
 
}
