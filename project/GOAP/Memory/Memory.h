#pragma once

#include "../../stdafx.h"
#include <Exam_HelperStructs.h>

class IExamInterface;

// Singleton class to store and manage persistent world knowledge


class SeenPurge
{
public:
	SeenPurge(const PurgeZoneInfo& zi);
	bool ZoneIsSafeAgain(float elapsedSec);
	PurgeZoneInfo GetPurgeInfo() const { return m_PurgeInfo; }
private:
	PurgeZoneInfo m_PurgeInfo;
	float m_SecondsSinceSeen = 0;
	float m_PurgeZoneMaxTimer = 7.f;
};

class VisitedHouse
{
public:
	VisitedHouse(const HouseInfo& hi);
	bool HasBeenForgotten(float elapsedSec);
	const HouseInfo& GetHouseInfo() const { return m_HouseInfo; }
private:
	HouseInfo m_HouseInfo;
	float m_SecondsSinceVisit = 0;
	float m_SecondsTillRefresh = 2000.f;
};

struct UnvisitedHouse
{
	UnvisitedHouse(const HouseInfo& hi) : HouseInfo(hi)
	{
		Elite::Vector2 size = hi.Size * SizeMultiplier;
		Elite::Vector2 flip = hi.Size * SizeMultiplier;
		flip.y = -flip.y;

		PointsToVisit.emplace_back(hi.Center - flip / 2.f);//top left
		PointsToVisit.emplace_back(hi.Center + size / 2.f);//top right
		PointsToVisit.emplace_back(hi.Center + flip / 2.f);//bottom right
		PointsToVisit.emplace_back(hi.Center - size / 2.f);//bottom left
		PointsToVisit.emplace_back(hi.Center);
	}
	UnvisitedHouse()
	{
	}
	std::vector<Elite::Vector2> PointsToVisit;
	HouseInfo HouseInfo;
	float SizeMultiplier = 0.5f;
	int MaxSize = 5;
};









class WorldMemory final
{
public:
	WorldMemory(const WorldMemory&) = delete;
	WorldMemory(WorldMemory&&) noexcept = delete;
	WorldMemory& operator=(const WorldMemory&) = delete;
	WorldMemory& operator=(WorldMemory&&) noexcept = delete;
	WorldMemory(WorldMemory& other) = delete;

	// Access & lifecycle
	static WorldMemory* Instance();
	static void Release();

	// Main update loop
	void Refresh(float deltaTime, IExamInterface* iface);

	// House tracking
	bool RegisterHouse(const HouseInfo& data);                  // Returns false if already stored
	bool HasSeenHouse(const HouseInfo& data) const;
	void FlagHouseVisited(const HouseInfo& data);
	bool HasVisitedHouse(const HouseInfo& data) const;
	size_t UnvisitedHouseCount() const;
	UnvisitedHouse* NearestUnvisitedHouse();                     // nullptr if none remain
	HouseInfo& ClosestKnownHouse();                              // Undefined if empty

	// Purge zone tracking
	bool RegisterPurgeZone(const PurgeZoneInfo& zoneData);       // Returns false if already stored
	bool HasSeenPurgeZone(const PurgeZoneInfo& zoneData) const;
	std::vector<PurgeZoneInfo> AllPurgeZones() const;

	// Item tracking
	bool RememberItem(const ItemInfo& item);
	bool ForgetItem(const ItemInfo& item);
	bool IsItemKnown(const ItemInfo& item) const;
	bool HasAnyItems() const;
	std::vector<ItemInfo> ItemsList() const { return m_Items; }

	// Status flags
	bool recentlyBitten = false;

private:
	WorldMemory();
	static WorldMemory* s_Instance;

	std::vector<VisitedHouse> m_VisitedHouses;
	std::vector<UnvisitedHouse> m_UnvisitedHouses;
	std::vector<HouseInfo> m_KnownHouses;
	std::vector<SeenPurge> m_PurgeRecords;
	std::vector<ItemInfo> m_Items;

	UnvisitedHouse m_LastNearestHouse{};
	int m_LastHouseIndex = -1;

	bool m_WasInsideHouse = false;

	float m_BiteTimer = 0.f;
	float m_BiteCooldown = 2.f;
	float m_HouseVisitProximity = 1.f;
};