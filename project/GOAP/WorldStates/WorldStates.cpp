#include "../../stdafx.h"
#include "WorldStates.h"
#include <IExamInterface.h>
#include "../Memory/Memory.h"
#include "../Utils/WorldUtils.h" 
#include <algorithm>
#include"Exam_HelperStructs.h"

void HasSavedUpFood::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::InventoryContains(iFace, eItemType::FOOD);
}

void HasSavedUpMedKits::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::InventoryContains(iFace, eItemType::MEDKIT);
}


void IsLoadedWithMedKits::Update(float elapsedSec, IExamInterface * iFace)
{
	int currentMedKitCount{ 0 };
	ItemInfo currentItem{};

	m_Predicate = false;

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::MEDKIT)
		{
			++currentMedKitCount;

			if (currentMedKitCount >= m_AcceptableAmountOfMedKits)
			{
				m_Predicate = true;
				return;
			}
		}
	}
}

void HasSavedWeaponsWithAcceptableAmmo::Update(float elapsedSec, IExamInterface* iFace)
{

	ItemInfo item;
	int numOfAcceptableWeapons = 0;
	m_Predicate = false;
	for (size_t i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, item) &&
			(item.Type == eItemType::PISTOL ||
				item.Type == eItemType::SHOTGUN) && item.Value >= m_AcceptableAmmo)
		{
			++numOfAcceptableWeapons;

			if (numOfAcceptableWeapons >= m_NumberOfAcceptableWeapons)
			{
				m_Predicate = true;
				return;
			}
		}
	}
}

void HasVisitedAllSeenHouses::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = WorldMemory::Instance()->HousesToVisitSize() == 0;
}

void HasWeaponState::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = WorldUtils::InventoryContains(iFace, eItemType::PISTOL) || WorldUtils::InventoryContains(iFace, eItemType::SHOTGUN);
}

void ThereAreHousesToVisit::Update(float elapsedSec, IExamInterface * iFace)
{
	m_Predicate = WorldMemory::Instance()->HousesToVisitSize() > 0;
}

void IsHungry::Update(float elapsedSec, IExamInterface* iFace)
{
	float totalEnergyValueInInventory{ 0 };
	float actualHungerThreshold{};

	if (WorldUtils::InventoryContains(iFace,eItemType::FOOD))
	{
		totalEnergyValueInInventory = WorldUtils::AddValueOfItem(iFace,eItemType::FOOD);

		if (totalEnergyValueInInventory > m_MaxEnergyCapacity)
		{
			actualHungerThreshold = totalEnergyValueInInventory - m_MaxEnergyCapacity;
		}
		else
		{
			actualHungerThreshold = m_LowestHungerThreshold;
		}
	}
	else
	{
		actualHungerThreshold = m_HungerThresholdIfNoFoodInInventory;
	}

	m_Predicate = iFace->Agent_GetInfo().Energy <= actualHungerThreshold;
}

void IsHurtState::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = iFace->Agent_GetInfo().Health < m_HealthThreshold;
}

void IsInHouseState::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = iFace->Agent_GetInfo().IsInHouse;
}

void IsInPurgeZoneState::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = false;

	std::vector<PurgeZoneInfo> purgeZonesInfo = iFace->GetPurgeZonesInFOV();

	for (const auto& purgeZone : purgeZonesInfo)
	{
		WorldMemory::Instance()->AddPurgeToMemory(purgeZone);
	}

	auto agentInfo = iFace->Agent_GetInfo();

	purgeZonesInfo = WorldMemory::Instance()->GetAllSeenPurges();

	for (auto& seenPurge : purgeZonesInfo)
	{
		if ((agentInfo.Position - seenPurge.Center).Magnitude() < (seenPurge.Radius + 5))
		{
			m_Predicate = true; 
			break;
		}
	}
}

void KnowsFoodLocation::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::KnowsItemLocation(iFace,{eItemType::FOOD});
}

void KnowsMedKitLocation::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::KnowsItemLocation(iFace, { eItemType::MEDKIT});
}

void KnowsWeaponLocation::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::KnowsItemLocation(iFace, { eItemType::SHOTGUN,eItemType::PISTOL});
}

void NextToFood::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::NextToItem(iFace,{eItemType::FOOD});
}

void NextToMedKit::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::NextToItem(iFace, {eItemType::MEDKIT});
}

void NextToWeapon::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldUtils::NextToItem(iFace, { eItemType::PISTOL,eItemType::SHOTGUN});
}

void RecentlyBittenState::Update(float elapsedSec, IExamInterface* iFace)
{
	if (iFace->Agent_GetInfo().WasBitten)
		m_GracePeriod = m_DefaultGracePeriod;

	if (m_GracePeriod > 0)
	{
		m_GracePeriod -= elapsedSec;
		m_Predicate = true;
	}
	else
	{
		m_Predicate = false;
	}
}

void ZombieInViewState::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = iFace->FOV_GetStats().NumEnemies > 0;
}
