
#include"Actions.h"
#include"../WorldStates/WorldStates.h"
#include<vector>
#include <IExamInterface.h>
#include"../Memory/Memory.h"
#include"../Utils/WorldUtils.h"


ConsumeSavedFood::ConsumeSavedFood()
{
	SetName (typeid(this).name());

	AddPrecondition(new HasSavedUpFood(true));  
	AddEffect(new IsHungry(false));

}

bool ConsumeSavedFood::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();

	if (WorldUtils::IsAttributeHigherThanMax(agentInfo.Energy))
	{
		return false;
	}

	return WorldUtils::ConsumeItem(iFace, eItemType::FOOD);


}
ConsumeSavedMedKit::ConsumeSavedMedKit()
{
	SetName(typeid(this).name());

	AddPrecondition(new HasSavedUpMedKits(true));
	AddEffect(new IsHurtState(false));

}

bool ConsumeSavedMedKit::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();
	if (WorldUtils::IsAttributeHigherThanMax(agentInfo.Health))
	{
		return false;
	}

	return WorldUtils::ConsumeItem(iFace, eItemType::MEDKIT);

}

GoToNearestSeenFood::GoToNearestSeenFood()
{
	SetName(typeid(this).name());
	AddPrecondition(new HasSavedUpFood(false));
	AddPrecondition(new KnowsFoodLocation(true));
	AddPrecondition(new NextToFood(false));
	AddEffect(new NextToFood(true));
}

bool GoToNearestSeenFood::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	return WorldUtils::GoToNearestSeenItem(iFace,eItemType::FOOD, steeringOutput);
}

GoToNearestSeenGun::GoToNearestSeenGun()
{

	SetName(typeid(this).name());

	AddPrecondition(new KnowsWeaponLocation(true));
	AddPrecondition(new NextToWeapon(false));
	AddEffect(new NextToWeapon(true));
}

bool GoToNearestSeenGun::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	return WorldUtils::GoToNearestSeenItem(iFace, eItemType::PISTOL, steeringOutput,eItemType::SHOTGUN);

}

GoToNearestSeenMedKit::GoToNearestSeenMedKit()
{
	SetName(typeid(this).name());

	AddPrecondition(new KnowsMedKitLocation(true));
	AddPrecondition(new NextToMedKit(false));
	AddEffect(new NextToMedKit(true));
}

bool GoToNearestSeenMedKit::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	return WorldUtils::GoToNearestSeenItem(iFace, eItemType::MEDKIT, steeringOutput);

}

LeaveHouse::LeaveHouse()
{

	SetName(typeid(this).name());

	AddPrecondition(new IsInHouseState(true));
	AddEffect(new IsInHouseState(false));
}

bool LeaveHouse::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();

	auto dir = (Elite::Vector2(cosf(agentInfo.Orientation - static_cast<float>(M_PI / 2)), sinf(agentInfo.Orientation - static_cast<float>(M_PI / 2)))).GetNormalized() + agentInfo.Position;
	auto target = iFace->NavMesh_GetClosestPathPoint(dir * 10);
	iFace->Draw_Circle(dir * 10, 2, Elite::Vector3(0, 0, 1), 0.9f);

	steeringOutput.LinearVelocity = (target - agentInfo.Position).GetNormalized() * agentInfo.MaxLinearSpeed;


	std::vector<HouseInfo> houseInfos = iFace->GetHousesInFOV();

	for (size_t i = 0; i < houseInfos.size(); i++)
	{
		WorldMemory::Instance()->AddHouseToMemory(houseInfos[i]);
	}

	return true;
}

MoveIntoHouse::MoveIntoHouse()
{

	SetName(typeid(this).name());

	AddPrecondition(new ThereAreHousesToVisit(true));

	AddEffect(new KnowsWeaponLocation(true));
	AddEffect(new KnowsMedKitLocation(true));
	AddEffect(new KnowsFoodLocation(true));
	AddEffect(new HasVisitedAllSeenHouses(true));
}

bool MoveIntoHouse::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	auto agentInfo = iFace->Agent_GetInfo();
	UnvisitedHouse * closestUnvisitedHouse{ WorldMemory::Instance()->GetClosestUnvisitedHouse()};



	if (closestUnvisitedHouse && !closestUnvisitedHouse->PointsToVisit.empty())
	{
		Elite::Vector2 target{};
		Elite::Vector2 realDelta{};
		if (closestUnvisitedHouse->PointsToVisit.size() == closestUnvisitedHouse->MaxSize)
		{
			target = iFace->NavMesh_GetClosestPathPoint(closestUnvisitedHouse->PointsToVisit.back());

			realDelta = closestUnvisitedHouse->PointsToVisit.back() - agentInfo.Position;

			if (realDelta.Magnitude() < m_PermittedDistanceFromHouseCenter)
			{
				closestUnvisitedHouse->PointsToVisit.pop_back();
			}
		}
		else
		{
			bool isInDesiredHouse{ false };
			std::vector<HouseInfo> housesInFOV = iFace->GetHousesInFOV();

			for (size_t i = 0; i < housesInFOV.size(); i++)
			{
				if ((housesInFOV.at(i).Center - closestUnvisitedHouse->HouseInfo.Center).Magnitude() < 1)
				{
					isInDesiredHouse = true;
					break;
				}
			}

			target = !agentInfo.IsInHouse || !isInDesiredHouse ?
				iFace->NavMesh_GetClosestPathPoint(closestUnvisitedHouse->PointsToVisit.back()) :
				closestUnvisitedHouse->PointsToVisit.back();

			realDelta = closestUnvisitedHouse->PointsToVisit.back() - agentInfo.Position;

			if (realDelta.Magnitude() < m_PermittedDistanceFromHouseCorners)
			{
				closestUnvisitedHouse->PointsToVisit.pop_back();
			}
		}
		steeringOutput.AutoOrient = false;
		steeringOutput.AngularVelocity = 360;
		Elite::Vector2 followDelta = (target - agentInfo.Position);

		steeringOutput.LinearVelocity = followDelta.GetNormalized() * agentInfo.MaxLinearSpeed;
		iFace->Draw_Circle(target, 2, Elite::Vector3(0, 1, 0));
		return true;
	}

	return false;
}

PickupFood::PickupFood()
{
	AddPrecondition(new NextToFood(true));
	AddEffect(new HasSavedUpFood(true));
	SetName(typeid(this).name());
}

bool PickupFood::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	return WorldUtils::PickUpItem(iFace);
}

PickupWeapon::PickupWeapon()
{

	AddPrecondition(new NextToWeapon(true));
	AddEffect(new HasWeaponState(true));
	AddEffect(new HasSavedWeaponsWithAcceptableAmmo(true));

	SetName(typeid(this).name());

}

bool PickupWeapon::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	return WorldUtils::PickUpItem(iFace);
}





RunFromPurge::RunFromPurge()
{

	AddPrecondition(new IsInPurgeZoneState(true));
	AddEffect(new IsInPurgeZoneState(false));
	SetName(typeid(this).name());

}

bool RunFromPurge::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	steeringOutput.RunMode = true;

	Elite::Vector2 target;
	auto allSeenPurges = WorldMemory::Instance()->GetAllSeenPurges();
	if (allSeenPurges.empty()) return false;
	PurgeZoneInfo purge = allSeenPurges[0];

	auto worldInfo = iFace->World_GetInfo();
	auto agentInfo = iFace->Agent_GetInfo();

	if ((worldInfo.Center - purge.Center).Magnitude() > (worldInfo.Center - agentInfo.Position).Magnitude())
		target = (worldInfo.Center - purge.Center).GetNormalized() * 1000;
	else
		target = (agentInfo.Position - purge.Center).GetNormalized() * 1000;
	target = iFace->NavMesh_GetClosestPathPoint(target);
	steeringOutput.LinearVelocity = (target - agentInfo.Position).GetNormalized() * agentInfo.MaxLinearSpeed * iFace->Agent_GetInfo().MaxLinearSpeed;
	iFace->Draw_Circle(target, 2, Elite::Vector3(0, 1, 0));

	return true;
}

ShootEnemyInView::ShootEnemyInView()
{

	AddPrecondition(new HasWeaponState(true));
	AddPrecondition(new ZombieInViewState(true));
	AddEffect(new ZombieInViewState(false));
	SetName(typeid(this).name());
}

bool ShootEnemyInView::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	auto agent = iFace->Agent_GetInfo();

	std::vector<EnemyInfo> enemiesInfo = iFace->GetEnemiesInFOV();

	if (enemiesInfo.empty())
	{
		return false;
	}

	Elite::Vector2 targetPosition = enemiesInfo[0].Location;

	iFace->Draw_Circle(targetPosition, 2, Elite::Vector3(1, 0, 0), 0.9f);

	Elite::Vector2 delta{ targetPosition - agent.Position };
	delta.Normalize();

	float targetAngle = std::atan2(delta.y, delta.x);
	float deltaRight{ targetAngle - agent.Orientation };
	float deltaLeft{ agent.Orientation - (targetAngle + (2.f * (float)M_PI)) };
	float chosenDelta = abs(deltaRight) < abs(deltaLeft) ? deltaRight : deltaLeft;

	ItemInfo currentItem{};
	int slotToUse{ -1 };


	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem))
		{
			if (currentItem.Type == eItemType::SHOTGUN && currentItem.Value > 0.f)
			{
				slotToUse = i;
				break;
			}
			else if (currentItem.Type == eItemType::PISTOL)
			{
				slotToUse = i;
			}
		}

	}

	bool canShootPistol{ currentItem.Type == eItemType::PISTOL && abs(chosenDelta) <= Elite::ToRadians(m_AcceptableRangeToShootIfPistol) };
	bool canShootShotgun{ currentItem.Type == eItemType::SHOTGUN && abs(chosenDelta) <= Elite::ToRadians(m_AcceptableRangeToShootIfShotGun) };

	if (slotToUse >= 0 && (canShootPistol || canShootShotgun))
	{
		iFace->Inventory_UseItem(slotToUse);
	}

	steeringOutput.AutoOrient = false;
	steeringOutput.AngularVelocity = chosenDelta / elapsedSec;
	return true;
}

SprintAwayFromEnemy::SprintAwayFromEnemy()
{
	AddPrecondition(new HasWeaponState(false));
	AddPrecondition(new KnowsWeaponLocation(false));
	AddPrecondition(new ZombieInViewState(true));

	AddEffect(new ZombieInViewState(false));
   SetName(typeid(this).name());
}

bool SprintAwayFromEnemy::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	auto agent = iFace->Agent_GetInfo();

	std::vector<EnemyInfo> enemiesInfo = iFace->GetEnemiesInFOV();

	if (enemiesInfo.empty())
	{
		return false;
	}

	float currentDistance = 0;
	float nearestDistance = FLT_MAX;
	int chosenIdx{ 0 };

	for (size_t i = 0; i < enemiesInfo.size(); i++)
	{
		currentDistance = (enemiesInfo[i].Location - agent.Position).MagnitudeSquared();

		if (currentDistance < nearestDistance)
		{
			chosenIdx = i;
			nearestDistance = currentDistance;
		}
	}

	Elite::Vector2 targetPosition = enemiesInfo[chosenIdx].Location;

	iFace->Draw_Circle(targetPosition, 2, Elite::Vector3(1, 0, 0), 0.9f);

	Elite::Vector2 delta{ targetPosition - agent.Position };
	delta.Normalize();

	float targetAngle = std::atan2(delta.y, delta.x);
	float deltaRight{ targetAngle - agent.Orientation };
	float deltaLeft{ agent.Orientation - (targetAngle + (2.f * (float)M_PI)) };
	float chosenDelta = abs(deltaRight) < abs(deltaLeft) ? deltaRight : deltaLeft;

	steeringOutput.RunMode = true;
	steeringOutput.AutoOrient = false;
	steeringOutput.AngularVelocity = chosenDelta / elapsedSec;
	steeringOutput.LinearVelocity = -delta * agent.MaxLinearSpeed;
	return true;
}




TurnReallyFast::TurnReallyFast()
{

    AddPrecondition(new HasWeaponState(false));
	AddPrecondition(new KnowsWeaponLocation(false));
	AddPrecondition(new ZombieInViewState(false));
	AddPrecondition(new RecentlyBittenState(true));

	AddEffect(new RecentlyBittenState(false));
    SetName(typeid(this).name());
}

bool TurnReallyFast::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	auto agent = iFace->Agent_GetInfo();

	Elite::Vector2 direction{};
	direction.x = cos(agent.Orientation);
	direction.y = sin(agent.Orientation);

	steeringOutput.AutoOrient = false;
	steeringOutput.AngularVelocity = agent.FOV_Angle / elapsedSec;
	//iFace->Agent_GetInfo().RunMode = true;
	return true;
}


Wander::Wander()
{
	SetName(typeid(this).name());
	m_StartingDegreeFromCenterForWanderPos = Elite::randomFloat(static_cast<float>(E_PI) * 2);
	SetWeight(100);
}

bool Wander::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	
	// --- Flashy Debug Additions ---
	float time = elapsedSec; // assumes world gives time
	
	// Velocity direction preview (cyan arrow)
	Elite::Vector2 forward = iFace->Agent_GetInfo().LinearVelocity.GetNormalized();


	// Small crosshair on wander pos (red)
	iFace->Draw_Direction(m_WanderPos, Elite::Vector2(6, 0), 1.f, Elite::Vector3(1, 0, 0));
	iFace->Draw_Direction(m_WanderPos, Elite::Vector2(-6, 0), 1.f, Elite::Vector3(1, 0, 0));
	iFace->Draw_Direction(m_WanderPos, Elite::Vector2(0, 6), 1.f, Elite::Vector3(1, 0, 0));
	iFace->Draw_Direction(m_WanderPos, Elite::Vector2(0, -6), 1.f, Elite::Vector3(1, 0, 0));




	iFace->Draw_Direction(m_WanderPos, iFace->Agent_GetInfo().Position,1000.f, Elite::Vector3(1, 0, 0));
	steeringOutput.AutoOrient = false;
	auto worldInfo = iFace->World_GetInfo();
	auto agent = iFace->Agent_GetInfo();
	auto agentPos = agent.Position;

	bool outOfBounds =
		agentPos.y > worldInfo.Dimensions.y / 2 ||
		agentPos.x > worldInfo.Dimensions.x / 2 ||
		agentPos.y < -worldInfo.Dimensions.y / 2 ||
		agentPos.x < -worldInfo.Dimensions.x / 2;

	if (outOfBounds || (m_WanderPos - agentPos).Magnitude() < m_DistanceFromWanderPosToHaveNewWanderPos)
	{
		NewIncrementalWanderPosInSpiral(worldInfo);
	}

	auto closestPathPoint = iFace->NavMesh_GetClosestPathPoint(m_WanderPos);

	if (NewWanderPosAwayFromPurges(closestPathPoint))
	{
		closestPathPoint = iFace->NavMesh_GetClosestPathPoint(m_WanderPos);
	}

	iFace->Draw_Circle(m_WanderPos, 2, Elite::Vector3(1, 0, 0));
	iFace->Draw_Circle(closestPathPoint, 2, Elite::Vector3(0, 1, 0));
	steeringOutput.LinearVelocity = (closestPathPoint - agentPos) * iFace->Agent_GetInfo().MaxLinearSpeed;
	steeringOutput.AngularVelocity = 360;



	ChangeColor(elapsedSec);

	iFace->Draw_Direction(iFace->Agent_GetInfo().Position, forward , 1000.f,*m_ColorPtr);
	float pulse = (sin(time * 4.f) * 0.5f + 0.5f) * 4.f + 2.f;
	iFace->Draw_Circle(iFace->Agent_GetInfo().Position, pulse, Elite::Vector3(0, 0.5f, 1));
	return true;





	

}

void Wander::ChangeColor(float elapsedSec)
{
	m_Timer += elapsedSec;


	if (m_Timer >= m_flipTime)
	{
		m_Timer = 0;

		if (m_ColorPtr == &m_Color1)
		{
			m_ColorPtr = &m_Color2;
		}
		else
		{
			m_ColorPtr = &m_Color1;

		}

	}
}

void Wander::NewRandomWanderPosInSpiral(const WorldInfo& worldPos)
{
	float maxWanderDistance = (worldPos.Dimensions / 2.f).Magnitude();

	m_WanderDir = Elite::Vector2(cos(m_StartingDegreeFromCenterForWanderPos), sin(m_StartingDegreeFromCenterForWanderPos));

	float wanderDistance =
		Elite::randomFloat(
			m_MinWanderDistanceMultiplier * maxWanderDistance,
			m_MaxWanderDistanceMultiplier * maxWanderDistance);

	m_WanderPos = m_WanderDir * wanderDistance + worldPos.Center;

	m_StartingDegreeFromCenterForWanderPos += Elite::randomFloat(m_MinDegreeIncrease, m_MaxDegreeIncrease);
	std::cout << "******New wander direction********\n";
}

void Wander::NewIncrementalWanderPosInSpiral(const WorldInfo& worldPos)
{
	float currentWanderDistance = ((worldPos.Dimensions / 2.f).Magnitude()) * m_CurrentMultiplier;

	m_WanderDir = Elite::Vector2(cos(m_StartingDegreeFromCenterForWanderPos), sin(m_StartingDegreeFromCenterForWanderPos));

	m_WanderPos = m_WanderDir * currentWanderDistance + worldPos.Center;

	float unitCirc = (2 * M_PI);
	float increase = Elite::randomFloat(m_MinDegreeIncrease, m_MaxDegreeIncrease);
	m_CompoundDegreeFromCenterForWanderPos += increase;
	m_StartingDegreeFromCenterForWanderPos += increase;

	if (m_CompoundDegreeFromCenterForWanderPos >= unitCirc)
	{
		m_CompoundDegreeFromCenterForWanderPos -= unitCirc;

		m_CurrentMultiplier += m_CurrentDeltaInMultiplierPerTime;

		if (m_CurrentMultiplier >= m_MaxWanderDistanceMultiplier)
		{
			m_CurrentMultiplier = m_MaxWanderDistanceMultiplier - m_DeltaInMultiplierPerTime;

			m_CurrentDeltaInMultiplierPerTime = -m_DeltaInMultiplierPerTime;
		}
		else if (m_CurrentMultiplier <= m_MinWanderDistanceMultiplier)
		{

			m_CurrentMultiplier = m_MinWanderDistanceMultiplier + m_DeltaInMultiplierPerTime;

			m_CurrentDeltaInMultiplierPerTime = m_DeltaInMultiplierPerTime;
		}
	}


	std::cout << "******New wander direction********\n";
}

bool Wander::NewWanderPosAwayFromPurges(const Elite::Vector2& targetPos)
{
	std::vector<PurgeZoneInfo> purges = WorldMemory::Instance()->GetAllSeenPurges();

	if (!purges.empty())
	{
		bool targetPosIsTooClose = false;

		Elite::Vector2 averagePosFromCenter;
		Elite::Vector2 currentDelta;

		for (size_t i = 0; i < purges.size(); i++)
		{
			currentDelta = (purges[i].Center - targetPos);

			if (currentDelta.Magnitude() < (purges[i].Radius + m_MinDistanceFromPurge))
			{
				averagePosFromCenter += purges[i].Center;
				targetPosIsTooClose = true;
			}
		}

		if (!targetPosIsTooClose)
		{
			return false;
		}

		averagePosFromCenter /= (float)purges.size();

		m_WanderDir = averagePosFromCenter - targetPos;

		m_WanderPos = m_WanderDir.GetNormalized() * m_OptimalDistanceFromPurge + averagePosFromCenter;

		m_StartingDegreeFromCenterForWanderPos += Elite::randomFloat(m_MinDegreeIncrease, m_MaxDegreeIncrease);
		std::cout << "******New wander direction********\n";

		return true;
	}


	return false;
}

void Wander::ToogleColor()
{




}


PickupMedKit::PickupMedKit()
{

	AddPrecondition(new NextToMedKit(true));

	AddEffect(new HasSavedUpMedKits(true));
	AddEffect(new IsLoadedWithMedKits(true));
	SetName(typeid(this).name());
}

bool PickupMedKit::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	std::vector<ItemInfo> itemInfos = iFace->GetItemsInFOV();

	int selectedIndex = 0;
	bool pickedUp = false;

	for (const auto& itemInfo : itemInfos)
	{
		if ((itemInfo.Location - iFace->Agent_GetInfo().Position).Magnitude() < iFace->Agent_GetInfo().GrabRange)
		{
			pickedUp = false;

			switch (itemInfo.Type)
			{

			case eItemType::MEDKIT:


				selectedIndex = GetMedKitSelectedInventoryIndex(itemInfo, iFace);

				if (selectedIndex >= 0) //was picked is succesf
				{
					pickedUp = true;
					iFace->GrabItem(itemInfo);
					iFace->Inventory_AddItem(selectedIndex, itemInfo);
				}
				break;
			   default:
				break;
			}

			if (pickedUp)
			{
				WorldMemory::Instance()->RemoveItemFromMemory(itemInfo);
			}
		}
	}
	return false;
}

int PickupMedKit::GetMedKitSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace)
{

	ItemInfo currentItem{};

	for (UINT i = m_StartMedKitInvSlot; i <= m_LastMedKitInvSlot; i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::MEDKIT)
		{ //already a med kit there 
			continue;
		}
		else
		{
			iFace->Inventory_RemoveItem(i);
			return i;
		}

	}
	return -1;
}
