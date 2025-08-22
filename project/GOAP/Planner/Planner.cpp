#include"Planner.h"
#include"../Actions/Actions.h"
#include"../WorldStates/WorldStates.h"

Planner::Planner(const std::vector<std::unique_ptr<BaseWorldState>>& pWorldStates) :
	m_pWorldStates{ std::move(m_pWorldStates) }
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

void Planner::MakeGraph(BaseWorldState* stateToAchieve)
{
}
