#pragma once
#include "../../stdafx.h"
#include <Exam_HelperStructs.h>
#include "VisitedHouse.h"

class SeenPurge;
class IExamInterface;

class WorldMemory
{
public:
	WorldMemory(const WorldMemory&) = delete;
	WorldMemory(WorldMemory&&) noexcept = delete;
	WorldMemory& operator=(const WorldMemory&) = delete;
	WorldMemory& operator=(WorldMemory&&) noexcept = delete;
	WorldMemory(WorldMemory& other) = delete;

	static WorldMemory* Instance();
	static void Destroy();
	void Update(float elapsedSec, IExamInterface* iFace);

	//Returns false if house is already in memory
	bool AddHouseToMemory(const HouseInfo& hi);
	bool IsHouseInMemory(const HouseInfo& hi);
	void MarkHouseAsVisited(const HouseInfo& hi);
	bool IsHouseVisited(const HouseInfo& hi);

	//Returns false if purge is already in memory
	bool AddPurgeToMemory(PurgeZoneInfo zi);
	bool IsPurgeInMemory(PurgeZoneInfo zi);
	std::vector<PurgeZoneInfo> GetAllSeenPurges();

	bool AddItemToMemory(const ItemInfo& item);
	bool RemoveItemFromMemory(const ItemInfo& item);
	bool IsItemInMemory(const ItemInfo& item);
	bool HasItems();
	std::vector<ItemInfo> GetAllItems() { return Instance()->m_ItemsSeen; }

	size_t HousesToVisitSize();
	UnvisitedHouse* GetClosestUnvisitedHouse(); //returns nullptr if no houses to visit
	HouseInfo & GetClosestHouseInMemory(); //returns nullptr if no houses to visit


	bool m_RecentlyBitten = false;

private:


	WorldMemory();
	static WorldMemory* m_Instance;

	std::vector<VisitedHouse> m_HousesVisited;
	std::vector<UnvisitedHouse> m_HousesToVisit;
	std::vector<HouseInfo> m_HousesSeen;
	std::vector<SeenPurge> m_PurgesSeen;

	std::vector<ItemInfo> m_ItemsSeen;

	UnvisitedHouse m_LastClosestHouse;
	int m_LastChosenHouseIndex = -1;

	bool m_WasInHouseLastFrame = false;

	float m_TimeSinceBitten = 0;
	float m_BittenGracePeriod = 2.f;
	float m_VisitedHouseThreshold = 1.f;
};

