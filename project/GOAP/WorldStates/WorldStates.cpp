#include "../../stdafx.h"
#include "WorldStates.h"
#include <IExamInterface.h>
#include "../Memory/Memory.h"
#include "../Utils/WorldUtils.h" 
#include <algorithm>
#include"Exam_HelperStructs.h"

void HasSavedUpFood::Update(float elapsedSec, IExamInterface* iFace)
{

	int currentMedKitCount{ 0 };
	ItemInfo currentItem{};

	m_Predicate = false;

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::FOOD)
		{
			m_Predicate = true;
			return;
		}
	}
}

void HasSavedUpMedKits::Update(float elapsedSec, IExamInterface* iFace)
{

	int currentMedKitCount{ 0 };
	ItemInfo currentItem{};

	m_Predicate = false;

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::MEDKIT)
		{
			m_Predicate = true;
			return;
		}
	}

}


void IsLoadedWithMedKits::Update(float elapsedSec, IExamInterface* iFace)
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

	m_Predicate = WorldMemory::Instance()->HousesToVisitSize() == 0;//returns true if there are houses left to visit

}

void HasWeaponState::Update(float elapsedSec, IExamInterface* iFace)
{
	ItemInfo item{};

	m_Predicate = false;
	for (size_t i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, item) &&
			(item.Type == eItemType::PISTOL ||
				item.Type == eItemType::SHOTGUN))
		{
			m_Predicate = true;
			return;
		}
	}
}

void ThereAreHousesToVisit::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = WorldMemory::Instance()->HousesToVisitSize() > 0;

}

void IsHungry::Update(float elapsedSec, IExamInterface* iFace)
{
	float totalEnergyValueInInventory{ 0 };
	ItemInfo currentItem{};
	bool foodInInventory{ false };

	for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
	{
		if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::FOOD)
		{
			foodInInventory = true;
			totalEnergyValueInInventory += currentItem.Value;
		}
	}

	float actualHungerThreshold =
		!foodInInventory ? m_HungerThresholdIfNoFoodInInventory :
		totalEnergyValueInInventory >= m_MaxEnergyCapacity ? m_LowestHungerThreshold :
		m_MaxEnergyCapacity - totalEnergyValueInInventory;


	m_Predicate = iFace->Agent_GetInfo().Energy <= actualHungerThreshold;
}

void IsHurtState::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = iFace->Agent_GetInfo().Health < m_HealthThreshold;

}

void IsInHouseState::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate =
		iFace->Agent_GetInfo().IsInHouse;
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
			m_Predicate = true;  //
			break;
		}
	}
}

void KnowsFoodLocation::Update(float elapsedSec, IExamInterface* iFace)
{
	m_Predicate = false;

	std::vector<ItemInfo> itemInfo = WorldMemory::Instance()->GetAllItems();

	for (size_t i = 0; i < itemInfo.size(); i++)
	{
		if (itemInfo[i].Type == eItemType::FOOD)
		{
			m_Predicate = true;
			return;
		}
	}
}

void KnowsMedKitLocation::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = false;

	std::vector<ItemInfo> itemInfo = WorldMemory::Instance()->GetAllItems();

	for (size_t i = 0; i < itemInfo.size(); i++)
	{
		if (itemInfo[i].Type == eItemType::MEDKIT)
		{
			m_Predicate = true;
			return;
		}
	}
}

void KnowsWeaponLocation::Update(float elapsedSec, IExamInterface* iFace)
{

	m_Predicate = false;

	std::vector<ItemInfo> itemInfo = WorldMemory::Instance()->GetAllItems();

	for (size_t i = 0; i < itemInfo.size(); i++)
	{
		if (itemInfo[i].Type == eItemType::PISTOL || itemInfo[i].Type == eItemType::SHOTGUN)
		{
			m_Predicate = true;
			return;
		}
	}
}

void NextToFood::Update(float elapsedSec, IExamInterface* iFace)
{
	std::vector<ItemInfo> itemInfo = iFace->GetItemsInFOV();

	m_Predicate = false;
	for (const auto& info : itemInfo)
	{
		if (info.Type == eItemType::FOOD)
		{
			if ((info.Location - iFace->Agent_GetInfo().Position).Magnitude() < iFace->Agent_GetInfo().GrabRange)
			{
				m_Predicate = true;
				break;
			}
		}

	}
}

void NextToMedKit::Update(float elapsedSec, IExamInterface* iFace)
{

	std::vector<ItemInfo> itemInfo = iFace->GetItemsInFOV();

	m_Predicate = false;
	for (const auto& info : itemInfo)
	{
		if (info.Type == eItemType::MEDKIT)
		{
			if ((info.Location - iFace->Agent_GetInfo().Position).Magnitude() < iFace->Agent_GetInfo().GrabRange)
			{
				m_Predicate = true;
				break;
			}
		}

	}
}

void NextToWeapon::Update(float elapsedSec, IExamInterface* iFace)
{
	std::vector<ItemInfo> itemInfo = iFace->GetItemsInFOV();

	m_Predicate = false;
	for (const auto& info : itemInfo)
	{
		if (info.Type == eItemType::SHOTGUN ||
			info.Type == eItemType::PISTOL)
		{
			if ((info.Location - iFace->Agent_GetInfo().Position).Magnitude() < iFace->Agent_GetInfo().GrabRange)
			{
				m_Predicate = true;
				break;
			}
		}

	}
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
