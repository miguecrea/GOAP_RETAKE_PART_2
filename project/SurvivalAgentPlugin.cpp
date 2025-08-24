#include "stdafx.h"
#include "SurvivalAgentPlugin.h"
#include "IExamInterface.h"

#include"GOAP/WorldStates/WorldStates.h"
#include"GOAP/Memory/Memory.h"




using namespace std;

SurvivalAgentPlugin::~SurvivalAgentPlugin()
{

}

void SurvivalAgentPlugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{

	m_pInterface = static_cast<IExamInterface*>(pInterface);

	info.BotName = "MinionExam";
	info.Student_Name = "Miguel";
	info.Student_Class = "2DAEGD11E";
	info.LB_Password = "ggchavaLES!";





	m_WorldStates.push_back(new HasSavedUpFood						(false));
	m_WorldStates.push_back(new HasSavedUpMedKits					(false));
	m_WorldStates.push_back(new IsLoadedWithMedKits					(false));
	m_WorldStates.push_back(new HasSavedWeaponsWithAcceptableAmmo	(false));
	m_WorldStates.push_back(new KnowsMedKitLocation					(false));	
	m_WorldStates.push_back(new KnowsWeaponLocation					(false));
	m_WorldStates.push_back(new KnowsFoodLocation					(false));
	m_WorldStates.push_back(new NextToMedKit						(false));
	m_WorldStates.push_back(new NextToWeapon						(false));
	m_WorldStates.push_back(new NextToFood							(false));
	m_WorldStates.push_back(new RecentlyBittenState					(false));
	m_WorldStates.push_back(new ThereAreHousesToVisit					(false));
	m_WorldStates.push_back(new IsInHouseState						(false));
	m_WorldStates.push_back(new ZombieInViewState					(false));
	m_WorldStates.push_back(new HasWeaponState						(false));
	m_WorldStates.push_back(new IsHurtState							(false));
	m_WorldStates.push_back(new IsHungry							(false));
	m_WorldStates.push_back(new HasVisitedAllSeenHouses				(true));
	m_WorldStates.push_back(new IsInPurgeZoneState					(false));

	m_Planner = std::make_unique<Planner>(&m_WorldStates);


}

void SurvivalAgentPlugin::DllInit()
{
}

void SurvivalAgentPlugin::DllShutdown()
{
}

//Called only once, during initialization. Only works in DEBUG Mode
void SurvivalAgentPlugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; 
	params.RenderUI = true; 
	params.SpawnEnemies = true; 
	params.EnemyCount = 100; 
	params.GodMode = false; 
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = false;
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = false;
	params.ShowDebugItemNames = true;
	params.Seed = 0;
}


void SurvivalAgentPlugin::Update_Debug(float dt)
{
	



}

SteeringPlugin_Output SurvivalAgentPlugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();


	for (auto & state : m_WorldStates)
	{
		state->Update(dt, m_pInterface);
	}

	WorldMemory::Instance()->Update(dt, m_pInterface);



	auto hasToSteer = m_Planner->CalculateAction(dt, steering, m_pInterface);

	if (hasToSteer)
	{
		return steering;
	}
   


	auto agentInfo = m_pInterface->Agent_GetInfo();
	auto nextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Target);
	FOVStats stats = m_pInterface->FOV_GetStats();



	auto vItemsInFOV = m_pInterface->GetItemsInFOV();

	if (m_GrabItem)
	{
		ItemInfo item;
		if (m_pInterface->GrabNearestItem(item))
		{
			m_pInterface->Inventory_AddItem(m_InventorySlot, item);
		}
	}

	if (m_UseItem)
	{
		m_pInterface->Inventory_UseItem(m_InventorySlot);
		ItemInfo item{};
		m_pInterface->Inventory_GetItem(m_InventorySlot, item);
		std::cout << "Item value: " << item.Value;
	}

	if (m_RemoveItem)
	{
		//Remove an item from a inventory slot
		m_pInterface->Inventory_RemoveItem(m_InventorySlot);
	}

	if (m_DestroyItemsInFOV)
	{
		for (auto& item : vItemsInFOV)
		{
			m_pInterface->DestroyItem(item);
		}
	}

	steering.LinearVelocity = nextTargetPos - agentInfo.Position;
	steering.LinearVelocity.Normalize(); 
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;

	if (Distance(nextTargetPos, agentInfo.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
	}

	steering.AutoOrient = true;
	steering.RunMode = m_CanRun;

	m_GrabItem = false;
	m_UseItem = false;
	m_RemoveItem = false;
	m_DestroyItemsInFOV = false;

	return steering;



}

void SurvivalAgentPlugin::Render(float dt) const
{
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}


