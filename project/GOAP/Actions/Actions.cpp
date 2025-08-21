
#include"Actions.h"
#include"../WorldStates/WorldStates.h"
#include<vector>
#include <IExamInterface.h>
#include"../Memory/Memory.h"





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
		if (iFace->Inventory_GetItem(i, currentItem) &&
			currentItem.Type == eItemType::FOOD)
		{
			agentInfo.Energy += currentItem.Value;
			iFace->Inventory_UseItem(i);
			iFace->Inventory_RemoveItem(i);

			if (agentInfo.Energy >= m_MaxEnergy)
			{
				return false;
			}
		}
	}

	return false;
}


/////////////

EvadeEnemy::EvadeEnemy()
{
	SetName(typeid(this).name());

	AddPrecondition(std::make_unique<IsNearEnemy>(true));
	AddPrecondition(std::make_unique<IsLowOnAmmo>(true));


	AddEffect(std::make_unique<SafeFromEnemy>(true));

}

bool EvadeEnemy::Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace)
{

	if (!iFace) return false;

	const float grabRange = iFace->Agent_GetInfo().GrabRange;
	const float fleeSpeed = 5.0f;

	// Find closest enemy
	EnemyInfo closestEnemy{};


	bool enemyFound = false;
	float closestDistSq = FLT_MAX;

	auto enemies = iFace->GetEnemiesInFOV();
	for (auto& e : enemies)
	{
		float distSq = DistanceSquared(iFace->Agent_GetInfo().Position, e.Location);
		if (distSq < closestDistSq)
		{
			closestDistSq = distSq;
			closestEnemy = e;
			enemyFound = true;
		}
	}

	if (!enemyFound)
	{
		// No enemies found, stop moving
		steeringOutput.LinearVelocity = { 0.f, 0.f };
		return true;
	}

	if (closestDistSq > grabRange * grabRange)
	{
		steeringOutput.LinearVelocity = { 0.f, 0.f };
		return true;
	}

	Elite::Vector2 agentPos = iFace->Agent_GetInfo().Position;
	Elite::Vector2 enemyPos = closestEnemy.Location;
	Elite::Vector2 fleeDir = agentPos - enemyPos;

	if (fleeDir.MagnitudeSquared() > 0)
	{
		fleeDir.Normalize();
		steeringOutput.LinearVelocity = fleeDir * fleeSpeed;
	}
	else
	{
		steeringOutput.LinearVelocity = { 0.f, 0.f };
	}

	steeringOutput.AngularVelocity = 0.f;
	steeringOutput.AutoOrient = true;
	steeringOutput.RunMode = true;

	return true;
}



GoToNearestSeenItem::GoToNearestSeenItem(const eItemType& Item) :
	m_DesiredItem{ Item }
{

	AddPrecondition(std::make_unique<KnowsItemLocation>(true, m_DesiredItem));
	AddEffect(std::make_unique<NextToItem>(true, m_DesiredItem));


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

	// --- 1. Compute Wander Target ---
	float circleDistance = m_CircleDistance; // how far ahead of agent
	float circleRadius = m_CircleRadius;     // how wide the wander arc

	Elite::Vector2 forward = agent.LinearVelocity.GetNormalized();
	if (forward.Magnitude() < 0.01f) // fallback if standing still
		forward = Elite::Vector2(1, 0);

	Elite::Vector2 circleCenter = agent.Position + forward * circleDistance;

	// Random displacement on circle
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

