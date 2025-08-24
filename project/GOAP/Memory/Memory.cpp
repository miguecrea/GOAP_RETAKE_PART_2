#include "../../stdafx.h"
#include "Memory.h"

#include <IExamInterface.h>

#include "SeenPurge.h"

WorldMemory* WorldMemory::m_Instance = nullptr;

WorldMemory * WorldMemory::Instance()
{
	if (m_Instance == nullptr)
		m_Instance = new WorldMemory();
	return m_Instance;
}

void WorldMemory::Destroy()
{
	delete m_Instance;
	m_Instance = nullptr;
}

bool WorldMemory::AddHouseToMemory(const HouseInfo & hi)
{
	if (IsHouseInMemory(hi)) return false;

	m_HousesSeen.push_back(hi);
	m_HousesToVisit.push_back(UnvisitedHouse{ hi });
	return true;
}

bool WorldMemory::IsHouseInMemory(const HouseInfo & hi)
{

	if (hi.Size == Elite::Vector2{ 0,0 }) return true; //Invalid house

	for (const auto & seenHouse : m_HousesSeen)
	{
		if ((seenHouse.Center - hi.Center).Magnitude() < 1)	return true;
	}
	return false;
}

void WorldMemory::MarkHouseAsVisited(const HouseInfo& hi)
{

	if (!IsHouseVisited(hi))
	{
		AddHouseToMemory(hi);

		for (int i = 0; i < m_HousesToVisit.size(); i++)
		{
			if ((m_HousesToVisit[i].HouseInfo.Center - hi.Center).Magnitude() < 1)
			{
				if (m_LastChosenHouseIndex == i)
				{
					m_LastChosenHouseIndex = -1;
				}

				m_HousesToVisit.erase(m_HousesToVisit.begin() + i);
				break;
			}
		}

		m_HousesVisited.push_back(VisitedHouse(hi));
	}
}

bool WorldMemory::IsHouseVisited(const HouseInfo& hi)
{
	if (hi.Size == Elite::Vector2{ 0,0 }) return true; //Invalid house

	for (auto visitedHouse : m_HousesVisited)
	{
		if ((visitedHouse.GetHouseInfo().Center - hi.Center).Magnitude() < 1)	return true;
	}
	return false;
}

bool WorldMemory::AddPurgeToMemory(PurgeZoneInfo zi)
{
	if (IsPurgeInMemory(zi)) return false;
	m_PurgesSeen.push_back(SeenPurge(zi));
	return true;
}

bool WorldMemory::IsPurgeInMemory(PurgeZoneInfo zi)
{
	if (zi.ZoneHash == 0) return true; //Invalid purge

	for (auto seenPurge : m_PurgesSeen)
	{
		if (seenPurge.GetPurgeInfo().Center == zi.Center)	return true;
	}
	return false;
}

std::vector<PurgeZoneInfo> WorldMemory::GetAllSeenPurges()
{
	std::vector<PurgeZoneInfo> purges;
	for (auto purgeSeen : m_PurgesSeen)
	{
		purges.push_back(purgeSeen.GetPurgeInfo());
	}
	return purges;
}

bool WorldMemory::AddItemToMemory(const ItemInfo& item)
{
	if (IsItemInMemory(item)) return false;
	m_ItemsSeen.push_back(item);
	return true;
}

bool WorldMemory::RemoveItemFromMemory(const ItemInfo& item)
{
	for (size_t i = 0; i < m_ItemsSeen.size(); ++i)
	{
		if (m_ItemsSeen[i].Location == item.Location)
		{
			m_ItemsSeen.erase(m_ItemsSeen.begin() + i);
			return true;
		}
	}
	return false;
}

bool WorldMemory::IsItemInMemory(const ItemInfo& item)
{
	for (const auto& seenItem : m_ItemsSeen)
	{
		if (seenItem.ItemHash == item.ItemHash)	return true;
	}
	return false;
}

bool WorldMemory::HasItems()
{
	return !m_ItemsSeen.empty();
}

size_t WorldMemory::HousesToVisitSize()
{
	return m_HousesToVisit.size();
}

UnvisitedHouse* WorldMemory::GetClosestUnvisitedHouse()
{
	return m_HousesToVisit.empty() ? nullptr : &m_LastClosestHouse;
}

void WorldMemory::Update(float elapsedSec, IExamInterface* iFace)
{

	//--------------RECORD---------------
	std::vector<ItemInfo> itemsInfo = iFace->GetItemsInFOV();
	for (const auto& item : itemsInfo)
	{
		if (item.Type == eItemType::GARBAGE)
		{
			iFace->DestroyItem(item);
		}

		else if (AddItemToMemory(item))
		{
			switch (item.Type)
			{
			case eItemType::PISTOL:
				std::cout << "Saved PISTOL into memory\n";
				break;
			case eItemType::SHOTGUN:
				std::cout << "Saved SHOTGUN into memory\n";
				break;
			case eItemType::MEDKIT:
				std::cout << "Saved MEDKIT into memory\n";
				break;
			case eItemType::FOOD:
				std::cout << "Saved FOOD into memory\n";
				break;
			default:
				break;
			}
		}
	}

	std::vector<HouseInfo> houseInfo = iFace->GetHousesInFOV();
	for (const auto& house : houseInfo)
	{
		if (AddHouseToMemory(house))
		{
			std::cout << "Saved house into memory\n";
		}
	}

	//---------FORGET AFTER A WHILE--------------

	for (size_t i = 0; i < m_HousesVisited.size(); )
	{
		if (m_HousesVisited[i].HasBeenForgotten(elapsedSec))
		{
			m_HousesVisited.erase(m_HousesVisited.begin() + i);  //delete house 


			//
			m_HousesToVisit.push_back(UnvisitedHouse{ m_HousesVisited[i].GetHouseInfo() });
		}
		else
		{
			++i;
		}
	}

	for (size_t i = 0; i < m_PurgesSeen.size(); )
	{
		if (m_PurgesSeen[i].ZoneIsSafeAgain(elapsedSec))
		{
			m_PurgesSeen.erase(m_PurgesSeen.begin() + i);
		}
		else
		{
			++i;
		}
	}

	//---------RECENTLY BITTEN FOR CUSTOM GRACE PERIOD--------------

	auto agentInfo = iFace->Agent_GetInfo();

	if (agentInfo.WasBitten) m_RecentlyBitten = true;

	if (m_RecentlyBitten)
	{
		m_TimeSinceBitten += elapsedSec;
		if (m_TimeSinceBitten > m_BittenGracePeriod)
		{
			m_RecentlyBitten = false;
			m_TimeSinceBitten = 0;
		}
	}



	/*
		Get Closest House that you haven't visited
	*/


	m_WasInHouseLastFrame = agentInfo.IsInHouse;


	float currentDistance = 0;
	float nearestDistance = FLT_MAX;
	size_t chosenIndex{};

	if (!m_HousesToVisit.empty())
	{

		for (int i = 0; i < m_HousesToVisit.size(); i++)
		{
			currentDistance = (m_HousesToVisit[i].HouseInfo.Center - agentInfo.Position).MagnitudeSquared();


			if (currentDistance < nearestDistance)
			{
				chosenIndex = i;
				nearestDistance = currentDistance;
			}
		}
		if (m_LastChosenHouseIndex != chosenIndex)
		{
			if (m_LastChosenHouseIndex >= 0)
			{
				m_HousesToVisit[m_LastChosenHouseIndex] = m_LastClosestHouse;
			}
			m_LastChosenHouseIndex = chosenIndex;
			m_LastClosestHouse = m_HousesToVisit[chosenIndex];
		}

		if (m_LastClosestHouse.PointsToVisit.empty())
		{
			MarkHouseAsVisited(m_LastClosestHouse.HouseInfo);
		}
	}

	/*

		DEBUG DRAW

	*/
	for (const auto& house : m_HousesVisited)
	{
		iFace->Draw_Circle(house.GetHouseInfo().Center, 10, Elite::Vector3(1, 0, 0));
	}

	iFace->Draw_Circle(m_LastClosestHouse.HouseInfo.Center, 5, Elite::Vector3(0, 0, 1));

	for (auto item : m_ItemsSeen)
	{
		iFace->Draw_Circle(item.Location, 2, Elite::Vector3(1, 1, 0));
	}

}

WorldMemory::WorldMemory()
{

}
