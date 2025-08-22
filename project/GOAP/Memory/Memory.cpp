
#include "../../stdafx.h"
#include "Memory.h"

#include <IExamInterface.h>

WorldMemory * WorldMemory::s_Instance = nullptr;

WorldMemory * WorldMemory::Instance()
{
    if (!s_Instance)
        s_Instance = new WorldMemory();
    return s_Instance;
}

void WorldMemory::Release()
{
    delete s_Instance;
    s_Instance = nullptr;
}

// ----------------- HOUSE MANAGEMENT -----------------
bool WorldMemory::RegisterHouse(const HouseInfo& house)
{
    if (HasSeenHouse(house))
        return false;

    m_KnownHouses.emplace_back(house);
    m_UnvisitedHouses.push_back(UnvisitedHouse{ house });
    return true;
}

bool WorldMemory::HasSeenHouse(const HouseInfo& house) const
{
    if (house.Size == Elite::Vector2{ 0,0 })
        return true; // invalid data

    for (const auto& known : m_KnownHouses)
    {
        if ((known.Center - house.Center).Magnitude() < 1.f)
            return true;
    }
    return false;
}

void WorldMemory::FlagHouseVisited(const HouseInfo& house)
{
    if (HasVisitedHouse(house))
        return;

    RegisterHouse(house);

    for (size_t i = 0; i < m_UnvisitedHouses.size(); ++i)
    {
        if ((m_UnvisitedHouses[i].HouseInfo.Center - house.Center).Magnitude() < 1.f)
        {
            if (m_LastHouseIndex == static_cast<int>(i))
                m_LastHouseIndex = -1;

            m_UnvisitedHouses.erase(m_UnvisitedHouses.begin() + i);
            break;
        }
    }

    m_VisitedHouses.emplace_back(house);
}

bool WorldMemory::HasVisitedHouse(const HouseInfo& house) const
{
    if (house.Size == Elite::Vector2{ 0,0 })
        return true; // invalid data

    for (const auto& visited : m_VisitedHouses)
    {
        if ((visited.GetHouseInfo().Center - house.Center).Magnitude() < 1.f)
            return true;
    }
    return false;
}

size_t WorldMemory::UnvisitedHouseCount() const
{
    return m_UnvisitedHouses.size();
}

UnvisitedHouse* WorldMemory::NearestUnvisitedHouse()
{
    return m_UnvisitedHouses.empty() ? nullptr : &m_LastNearestHouse;
}

HouseInfo& WorldMemory::ClosestKnownHouse()
{
    return m_LastNearestHouse.HouseInfo; // assumes valid call
}

// ----------------- PURGE ZONE MANAGEMENT -----------------
bool WorldMemory::RegisterPurgeZone(const PurgeZoneInfo& zone)
{
    if (HasSeenPurgeZone(zone))
        return false;

    m_PurgeRecords.emplace_back(zone);
    return true;
}

bool WorldMemory::HasSeenPurgeZone(const PurgeZoneInfo& zone) const
{
    if (zone.ZoneHash == 0)
        return true; // invalid data

    for (const auto& purge : m_PurgeRecords)
    {
        if (purge.GetPurgeInfo().Center == zone.Center)
            return true;
    }
    return false;
}

std::vector<PurgeZoneInfo> WorldMemory::AllPurgeZones() const
{
    std::vector<PurgeZoneInfo> result;
    result.reserve(m_PurgeRecords.size());

    for (const auto& purge : m_PurgeRecords)
        result.push_back(purge.GetPurgeInfo());

    return result;
}

// ----------------- ITEM MANAGEMENT -----------------
bool WorldMemory::RememberItem(const ItemInfo& item)
{
    if (IsItemKnown(item))
        return false;

    m_Items.push_back(item);
    return true;
}

bool WorldMemory::ForgetItem(const ItemInfo& item)
{
    for (size_t i = 0; i < m_Items.size(); ++i)
    {
        if (m_Items[i].Location == item.Location)
        {
            m_Items.erase(m_Items.begin() + i);
            return true;
        }
    }
    return false;
}

bool WorldMemory::IsItemKnown(const ItemInfo& item) const
{
    for (const auto& stored : m_Items)
    {
        if (stored.ItemHash == item.ItemHash)
            return true;
    }
    return false;
}

bool WorldMemory::HasAnyItems() const
{
    return !m_Items.empty();
}

// ----------------- UPDATE LOOP -----------------
void WorldMemory::Refresh(float deltaTime, IExamInterface* iface)
{
    // ----------- Capture new items in sight -----------
    for (const auto& item : iface->GetItemsInFOV())
    {
        if (item.Type == eItemType::GARBAGE)
        {
            iface->DestroyItem(item);
            continue;
        }

        if (RememberItem(item))
        {
            switch (item.Type)
            {
            case eItemType::PISTOL:   std::cout << "Stored PISTOL\n"; break;
            case eItemType::SHOTGUN:  std::cout << "Stored SHOTGUN\n"; break;
            case eItemType::MEDKIT:   std::cout << "Stored MEDKIT\n"; break;
            case eItemType::FOOD:     std::cout << "Stored FOOD\n"; break;
            default: break;
            }
        }
    }

    // ----------- Capture new houses -----------
    for (const auto& house : iface->GetHousesInFOV())
    {
        if (RegisterHouse(house))
            std::cout << "Stored house\n";
    }

    // ----------- Forget visited houses over time -----------
    for (size_t i = 0; i < m_VisitedHouses.size(); )
    {
        if (m_VisitedHouses[i].HasBeenForgotten(deltaTime))
        {
            auto forgottenHouse = m_VisitedHouses[i].GetHouseInfo();
            m_VisitedHouses.erase(m_VisitedHouses.begin() + i);
            m_UnvisitedHouses.push_back(UnvisitedHouse{ forgottenHouse });
        }
        else
        {
            ++i;
        }
    }

    // ----------- Forget purge zones when safe -----------
    for (size_t i = 0; i < m_PurgeRecords.size(); )
    {
        if (m_PurgeRecords[i].ZoneIsSafeAgain(deltaTime))
            m_PurgeRecords.erase(m_PurgeRecords.begin() + i);
        else
            ++i;
    }

    // ----------- Recently bitten grace period -----------
    auto agent = iface->Agent_GetInfo();

    if (agent.WasBitten)
        recentlyBitten = true;

    if (recentlyBitten)
    {
        m_BiteTimer += deltaTime;
        if (m_BiteTimer > m_BiteCooldown)
        {
            recentlyBitten = false;
            m_BiteTimer = 0.f;
        }
    }

    m_WasInsideHouse = agent.IsInHouse;

    // ----------- Determine nearest unvisited house -----------
    if (!m_UnvisitedHouses.empty())
    {
        float nearestDistSq = FLT_MAX;
        size_t nearestIndex = 0;

        for (size_t i = 0; i < m_UnvisitedHouses.size(); ++i)
        {
            float distSq = (m_UnvisitedHouses[i].HouseInfo.Center - agent.Position).MagnitudeSquared();
            if (distSq < nearestDistSq)
            {
                nearestDistSq = distSq;
                nearestIndex = i;
            }
        }

        if (m_LastHouseIndex != static_cast<int>(nearestIndex))
        {
            if (m_LastHouseIndex >= 0)
                m_UnvisitedHouses[m_LastHouseIndex] = m_LastNearestHouse;

            m_LastHouseIndex = static_cast<int>(nearestIndex);
            m_LastNearestHouse = m_UnvisitedHouses[nearestIndex];
        }

        if (m_LastNearestHouse.PointsToVisit.empty())
            FlagHouseVisited(m_LastNearestHouse.HouseInfo);
    }

    // ----------- Debug drawing -----------
    for (const auto& visited : m_VisitedHouses)
        iface->Draw_Circle(visited.GetHouseInfo().Center, 10.f, Elite::Vector3(1, 0, 0));

    iface->Draw_Circle(m_LastNearestHouse.HouseInfo.Center, 5.f, Elite::Vector3(0, 0, 1));

    for (const auto& item : m_Items)
        iface->Draw_Circle(item.Location, 2.f, Elite::Vector3(1, 1, 0));
}

WorldMemory::WorldMemory() = default;



SeenPurge::SeenPurge(const PurgeZoneInfo& zi)
{
}

bool SeenPurge::ZoneIsSafeAgain(float elapsedSec)
{
    return false;
}

VisitedHouse::VisitedHouse(const HouseInfo& hi)
{
}

bool VisitedHouse::HasBeenForgotten(float elapsedSec)
{
    return false;
}
