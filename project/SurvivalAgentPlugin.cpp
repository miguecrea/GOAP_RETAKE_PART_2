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





	m_WorldStates.push_back(std::make_unique<HasSavedUpItem>(false, eItemType::FOOD));
	m_WorldStates.push_back(std::make_unique<HasSavedUpItem>(false, eItemType::MEDKIT));
	m_WorldStates.push_back(std::make_unique<IsLoadedWithMedKits>(false));
	m_WorldStates.push_back(std::make_unique<HasSavedWeaponsWithAcceptableAmmo>(false));
	m_WorldStates.push_back(std::make_unique<HasVisitedAllSeenHouses>(false));
	m_WorldStates.push_back(std::make_unique<HasWeaponState>(false));
	m_WorldStates.push_back(std::make_unique<SafeFromEnemy>(false));
	m_WorldStates.push_back(std::make_unique<HouseInViewState>(false));
	m_WorldStates.push_back(std::make_unique<IsHungry>(false));
	m_WorldStates.push_back(std::make_unique<IsInHouseState>(false));
	m_WorldStates.push_back(std::make_unique<IsInPurgeZoneState>(false));

	m_WorldStates.push_back(std::make_unique<KnowsItemLocation>(false,eItemType::FOOD));
	m_WorldStates.push_back(std::make_unique<KnowsItemLocation>(false,eItemType::MEDKIT));
	m_WorldStates.push_back(std::make_unique<KnowsItemLocation>(false,eItemType::PISTOL));
	m_WorldStates.push_back(std::make_unique<KnowsItemLocation>(false,eItemType::SHOTGUN));

	m_WorldStates.push_back(std::make_unique<NextToItem>(false,eItemType::SHOTGUN));
	m_WorldStates.push_back(std::make_unique<NextToItem>(false,eItemType::PISTOL));
	m_WorldStates.push_back(std::make_unique<NextToItem>(false,eItemType::MEDKIT));
	m_WorldStates.push_back(std::make_unique<NextToItem>(false,eItemType::FOOD));

	m_WorldStates.push_back(std::make_unique<IsInventoryFull>(false));
	m_WorldStates.push_back(std::make_unique<IsLowOnAmmo>(false));
	m_WorldStates.push_back(std::make_unique<IsNearEnemy>(false));
	m_WorldStates.push_back(std::make_unique<ZombieInViewState>(false));
	m_WorldStates.push_back(std::make_unique<HasOpenInventorySlot>(false));
	m_WorldStates.push_back(std::make_unique<IsInjured>(false));





	m_Planner = std::make_unique<Planner>(m_WorldStates);


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
	params.EnemyCount = 20; 
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
	//Demo Event Code
	//In the end your Agent should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update_Debug target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_X))
	{
		m_DestroyItemsInFOV = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_InventorySlot > 0)
			--m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (m_InventorySlot < 4)
			++m_InventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_InventorySlot, info);
		std::cout << (int)info.Type << std::endl;

	}



}

SteeringPlugin_Output SurvivalAgentPlugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();

	std::cout << "Hello bro\n";

	for (auto & state : m_WorldStates)
	{
		state->Update(dt, m_pInterface);
	}

	WorldMemory::Instance()->Refresh(dt, m_pInterface);



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


