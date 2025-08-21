#include "../../stdafx.h"
#include "WorldStates.h"
#include <IExamInterface.h>
#include "../Memory/Memory.h"
#include "../Utils/WorldUtils.h" 
#include <algorithm>
#include"Exam_HelperStructs.h"

void IsLoadedWithMedKits::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::CountItems(iFace, eItemType::MEDKIT) >= m_AcceptableAmountOfMedKits;
}

void HasSavedWeaponsWithAcceptableAmmo::Update(float elapsedSec, IExamInterface* iFace)
{
    int count = WorldUtils::CountItemsWithValue(iFace, eItemType::PISTOL, m_AcceptableAmmo) + WorldUtils::CountItemsWithValue(iFace, eItemType::SHOTGUN, m_AcceptableAmmo);

    m_Predicate = count >= m_NumberOfAcceptableWeapons;
}

void HasWeaponState::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::InventoryContains(iFace, eItemType::PISTOL) || WorldUtils::InventoryContains(iFace, eItemType::SHOTGUN);
}

void IsHungry::Update(float elapsedSec, IExamInterface* iFace)
{
    float totalEnergyValueInInventory = 0.f;
    ItemInfo item{};
    bool foodInInventory = false;


    for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
    {
        if (iFace->Inventory_GetItem(i, item) && item.Type == eItemType::FOOD)
        {
            foodInInventory = true;
            totalEnergyValueInInventory += item.Value;
            //add up all the energy value in inventory 
        }
    }

    // Decide dynamic hunger threshold
    float actualHungerThreshold = !foodInInventory ? m_HungerThresholdIfNoFoodInInventory : totalEnergyValueInInventory >= m_MaxEnergyCapacity ? m_LowestHungerThreshold : m_MaxEnergyCapacity - totalEnergyValueInInventory;

    m_Predicate = WorldUtils::IsHungry(iFace, actualHungerThreshold);
}

void IsHurtState::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::HasLowHealth(iFace, m_HealthThreshold);
}

void IsInHouseState::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = iFace->Agent_GetInfo().IsInHouse;
}

void ZombieInViewState::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = iFace->FOV_GetStats().NumEnemies > 0;
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

void IsInventoryFull::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::IsInventoryFull(iFace);
}

void IsLowOnAmmo::Update(float elapsedSec, IExamInterface* iFace)
{
    // True if ANY weapon is below threshold

    m_Predicate =
        WorldUtils::InventoryContains(iFace, eItemType::PISTOL, 0) && WorldUtils::CountItemsWithValue(iFace, eItemType::PISTOL, m_AmmoThreshold) == 0
        ||
        WorldUtils::InventoryContains(iFace, eItemType::SHOTGUN, 0) &&
        WorldUtils::CountItemsWithValue(iFace, eItemType::SHOTGUN, m_AmmoThreshold) == 0;



}

void IsNearEnemy::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::EnemyInRange(iFace, iFace->Agent_GetInfo().GrabRange);
}

void HasOpenInventorySlot::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::HasOpenSlot(iFace);
}

void SafeFromEnemy::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::EnemiesAllOutOfRange(iFace, iFace->Agent_GetInfo().GrabRange);
}

void IsInPurgeZoneState::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = false;

    auto purgeZones = iFace->GetPurgeZonesInFOV();
    for (const auto& pz : purgeZones)
        WorldMemory::Instance()->RegisterPurgeZone(pz);

    auto agentPos = iFace->Agent_GetInfo().Position;
    for (const auto& seenPurge : WorldMemory::Instance()->AllPurgeZones())
    {
        if ((agentPos - seenPurge.Center).Magnitude() < seenPurge.Radius + 5)
        {
            m_Predicate = true;
            break;
        }
    }
}

void HouseInViewState::Update(float elapsedSec, IExamInterface* iFace)
{
    // Assuming we want "can SEE house", not "inside"
    auto houses = iFace->GetHousesInFOV();
    m_Predicate = !houses.empty();
}

void HasVisitedAllSeenHouses::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldMemory::Instance()->UnvisitedHouseCount() == 0;
}

void HasSavedUpItem::Update(float elapsedSec, IExamInterface* iFace)
{
    m_Predicate = WorldUtils::InventoryContains(iFace, m_ItemType);
}

void KnowsItemLocation::Update(float elapsedSec, IExamInterface* iFace)
{
    auto items = WorldMemory::Instance()->ItemsList();
    m_Predicate = std::any_of(items.begin(), items.end(),
        [&](const ItemInfo& info) { return info.Type == m_Item; });
}

void NextToItem::Update(float elapsedSec, IExamInterface* iFace)
{
    auto items = iFace->GetItemsInFOV();
    Elite::Vector2 agentPos = iFace->Agent_GetInfo().Position;
    float grabRange = iFace->Agent_GetInfo().GrabRange;

    m_Predicate = std::any_of(items.begin(), items.end(), [&](const ItemInfo& info)
        {
            return info.Type == m_Item &&
                (info.Location - agentPos).Magnitude() < grabRange;
        });
}
