#include "stdafx.h"
#include "SurvivalAgentPlugin.h"
#include "IExamInterface.h"



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

	//-----GOAP-------


	return steering;
	
}

void SurvivalAgentPlugin::Render(float dt) const
{
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}


