
#include"Actions.h"
#include"../WorldStates/WorldStates.h"
#include<vector>
#include <IExamInterface.h>
#include"../Memory/Memory.h"
#include"../Utils/WorldUtils.h"





ConsumeSavedFood::ConsumeSavedFood()
{
	SetName(typeid(this).name());
	AddPrecondition(std::make_unique<HasSavedUpItem>(true, eItemType::FOOD));
	AddEffect(std::make_unique<IsHungry>(false));
}


bool ConsumeSavedFood::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();

	if (agentInfo.Energy >= m_MaxEnergy)
	{
		return false;
	}

	ItemInfo currentItem{};

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::FOOD)
		{
			agentInfo.Energy += currentItem.Value;

			iFace->Inventory_UseItem(i);
			iFace->Inventory_RemoveItem(i);

			if (agentInfo.Energy >= m_MaxEnergy)
			{
				return false;  //dont consume anymore 
			}
		}
	}

	return false;
}


/////////////

EvadeEnemy::EvadeEnemy()
{
	SetName(typeid(this).name());

	AddPrecondition(std::make_unique<HasWeaponState>(false));
	AddPrecondition(std::make_unique<KnowsItemLocation>(false,eItemType::PISTOL));
	AddPrecondition(std::make_unique<ZombieInViewState>(true));

	AddEffect(std::make_unique<ZombieInViewState>(false));

}

bool EvadeEnemy::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
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



GoToNearestSeenItem::GoToNearestSeenItem(const eItemType& Item) :
	m_DesiredItem{ Item }
{

	AddPrecondition(std::make_unique<KnowsItemLocation>(true, m_DesiredItem));
	AddEffect(std::make_unique<NextToItem>(true, m_DesiredItem));

	//add name 
	switch (m_DesiredItem)
	{
	case eItemType::PISTOL:

	case eItemType::SHOTGUN:
		break;
	case eItemType::MEDKIT:

		break;
	case eItemType::FOOD:


		break;
	default:
		break;

	}
}

bool GoToNearestSeenItem::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	std::vector<ItemInfo> seenItems = WorldMemory::Instance()->ItemsList();

	std::vector<PurgeZoneInfo> seenPurges = WorldMemory::Instance()->AllPurgeZones();

	if (seenItems.empty())
	{
		return false;
	}

	auto agentInfo = iFace->Agent_GetInfo();
	float currentDistance = 0;
	float nearestDistance = FLT_MAX;
	bool itemInPurgeZone = false;
	bool chosenItemInPurgeZone = false;
	Elite::Vector2 target{};


	for (size_t i = 0; i < seenItems.size(); i++)
	{
		if (seenItems[i].Type == m_DesiredItem)
		{
			itemInPurgeZone = false;

			for (size_t i = 0; i < seenPurges.size(); i++)
			{
				if ((seenItems[i].Location - agentInfo.Position).MagnitudeSquared() <
					(seenPurges[i].Radius * seenPurges[i].Radius))
				{
					itemInPurgeZone = true;
					break;
				}
			}

			currentDistance = (seenItems[i].Location - agentInfo.Position).MagnitudeSquared();

			if (currentDistance < nearestDistance || (!itemInPurgeZone && chosenItemInPurgeZone))
			{
				chosenItemInPurgeZone = itemInPurgeZone;
				target = seenItems[i].Location;
				nearestDistance = currentDistance;
			}
		}
	}

	target = iFace->NavMesh_GetClosestPathPoint(target);
	steeringOutput.LinearVelocity = (target - agentInfo.Position).GetNormalized() * agentInfo.MaxLinearSpeed;
	iFace->Draw_Circle(target, 2, Elite::Vector3(0, 1, 0));

	return true;
}

Wander::Wander()
{

	SetName(typeid(this).name());

	m_WanderAngle = Elite::randomFloat(-E_PI, E_PI);
}

bool Wander::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{
	steeringOutput.AutoOrient = true;
	auto agent = iFace->Agent_GetInfo();

	float circleDistance = m_CircleDistance;
	float circleRadius = m_CircleRadius;

	Elite::Vector2 forward = agent.LinearVelocity.GetNormalized();
	if (forward.Magnitude() < 0.01f)
		forward = Elite::Vector2(1, 0);

	Elite::Vector2 circleCenter = agent.Position + forward * circleDistance;

	m_WanderAngle += Elite::randomFloat(-m_AngleChange, m_AngleChange);
	Elite::Vector2 offset{ cos(m_WanderAngle) * circleRadius, sin(m_WanderAngle) * circleRadius };

	Elite::Vector2 wanderTarget = circleCenter + offset;



	Elite::Vector2 desiredVelocity = (wanderTarget - agent.Position).GetNormalized() * agent.MaxLinearSpeed;
	steeringOutput.LinearVelocity = desiredVelocity;

	Elite::Vector2 toTarget = (wanderTarget - agent.Position).GetNormalized();
	float angle = atan2f(toTarget.y, toTarget.x);
	steeringOutput.AngularVelocity = Elite::ToDegrees(angle - agent.Orientation);

	// Debug draw
	iFace->Draw_Circle(circleCenter, circleRadius, Elite::Vector3(0, 1, 0));
	iFace->Draw_Circle(wanderTarget, 2, Elite::Vector3(1, 0, 0));

	return true;
}

ShootEnemy::ShootEnemy()
{
	SetName(typeid(this).name());

	AddPrecondition(std::make_unique<HasWeaponState>(true));
	AddPrecondition(std::make_unique<ZombieInViewState>(true));

	AddEffect(std::make_unique<ZombieInViewState>(false));
}

bool ShootEnemy::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
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
			if (currentItem.Type == eItemType::SHOTGUN)
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

	if (slotToUse >= 0 &&
		(canShootPistol || canShootShotgun))
	{
		iFace->Inventory_UseItem(slotToUse);
	}

	steeringOutput.AutoOrient = false;
	steeringOutput.AngularVelocity = chosenDelta / elapsedSec;
	return true;

}

PickUpItem::PickUpItem(const eItemType& Item) :
	m_DesiredItem{ Item }
{

	AddPrecondition(std::make_unique<NextToItem>(true, m_DesiredItem));
	AddPrecondition(std::make_unique<HasOpenInventorySlot>(true));

	AddEffect(std::make_unique<HasSavedUpItem>(true, m_DesiredItem));

}

bool PickUpItem::Execute(float elapsedSec, SteeringPlugin_Output & steeringOutput, IExamInterface * iFace)
{

	std::vector<ItemInfo> itemInfos = iFace->GetItemsInFOV();

	ItemInfo currentItem{};
	bool pickedUp = false;


	for (const auto& itemInfo : itemInfos)
	{
		if (itemInfo.Type == m_DesiredItem)
		{
			iFace->GrabItem(itemInfo);
			iFace->Inventory_AddItem(WorldUtils::GetFirstOpenSlot(iFace), itemInfo);
			pickedUp = true;
			WorldMemory::Instance()->ForgetItem(itemInfo);
			break;
		}
	}


	return false;
}


RunFromPurge::RunFromPurge()
{

	AddPrecondition(std::make_unique <IsInPurgeZoneState>(true));
	AddEffect(std::make_unique <IsInPurgeZoneState>(false));
}

bool RunFromPurge::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	steeringOutput.RunMode = true;

	Elite::Vector2 target;
	auto allSeenPurges = WorldMemory::Instance()->AllPurgeZones();
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

ConsumeSavedMedKit::ConsumeSavedMedKit()
{

	AddPrecondition(std::make_unique<HasSavedUpItem>(true,eItemType::MEDKIT));
	AddEffect(std::make_unique<IsHurtState>(false));

}

bool ConsumeSavedMedKit::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();

	if (agentInfo.Health >= m_MaxHealth)
	{
		return false;
	}

	ItemInfo currentItem{};

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) &&
			currentItem.Type == eItemType::MEDKIT)
		{
			agentInfo.Health += currentItem.Value;
			iFace->Inventory_UseItem(i);
			iFace->Inventory_RemoveItem(i);

			if (agentInfo.Health >= m_MaxHealth)
			{
				return false;
			}
		}
	}

	return false;

}

LeaveHouse::LeaveHouse()
{

	SetName(typeid(this).name());

	AddPrecondition(std::make_unique<IsInHouseState>(true));
	AddEffect(std::make_unique<IsInHouseState>(false));
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
		WorldMemory::Instance()->RegisterHouse(houseInfos[i]);
	}

	return true;
}

MoveIntoHouse::MoveIntoHouse()
{
	AddPrecondition(std::make_unique<HouseInViewState>(true));

	AddEffect(std::make_unique<KnowsItemLocation>(true,eItemType::FOOD));
	AddEffect(std::make_unique<KnowsItemLocation>(true,eItemType::MEDKIT));
	AddEffect(std::make_unique<KnowsItemLocation>(true,eItemType::PISTOL));
	AddEffect(std::make_unique<KnowsItemLocation>(true,eItemType::SHOTGUN));

	AddEffect(std::make_unique<HasVisitedAllSeenHouses>(true));

}

bool MoveIntoHouse::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	auto agentInfo = iFace->Agent_GetInfo();
	UnvisitedHouse* closestUnvisitedHouse{ WorldMemory::Instance()->NearestUnvisitedHouse()};



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
