#include"Planner.h"
#include"../Actions/Actions.h"
#include"../WorldStates/WorldStates.h"

Planner::Planner(const std::vector<std::unique_ptr<BaseWorldState>>& pWorldStates) :
	m_pWorldStates{ std::move(m_pWorldStates) }
{


	//m_Actions =
	//{
	//	std::make_unique<Wander>()
	//};

	m_Actions.push_back(std::make_unique<Wander>());







	//run from purge 
	// go to Item

	m_Goals =
	{
		std::make_unique<IsInPurgeZoneState>(false),
		std::make_unique<ZombieInViewState>(false),
		std::make_unique<HasWeaponState>(true),
		std::make_unique<IsHungry>(false),
		std::make_unique<HasSavedUpItem>(true,eItemType::FOOD),
		std::make_unique<IsLoadedWithMedKits>(true),
		std::make_unique<HasVisitedAllSeenHouses>(true),
		std::make_unique<HasSavedWeaponsWithAcceptableAmmo>(true),
		std::make_unique<SafeFromEnemy>(true),
		std::make_unique<IsInjured>(false),
		std::make_unique<IsLowOnAmmo>(false),
	};



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
}
