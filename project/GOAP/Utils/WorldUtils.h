


class IExamInterface;

#include"Exam_HelperStructs.h"

class WorldUtils
{
public:
	// Inventory helpers
	static bool InventoryContains(IExamInterface * iFace, eItemType type, int minValue = 0)
	{
		ItemInfo item{};
		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, item) && item.Type == type && item.Value >= minValue)
				return true;
		}
		return false;
	}

	static int CountItems(IExamInterface* iFace, eItemType type)
	{

		int count = 0;
		ItemInfo item{};
		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, item) && item.Type == type)
				++count;
		}
		return count;
	}
	static int CountItemsWithValue(IExamInterface* iFace, eItemType type, int minValue)
	{

		int count = 0;
		ItemInfo item{};

		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, item) && item.Type == type && item.Value >= minValue)
				++count;
		}
		return count;
	}


	static bool HasOpenSlot(IExamInterface * iFace)
	{

		ItemInfo item{};

		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (!iFace->Inventory_GetItem(i, item))
				return true;
		}
		return false;
	}

	static int GetFirstOpenSlot(IExamInterface * iFace)
	{

		ItemInfo item{};

		for (UINT InventoryIndex = 0; InventoryIndex < iFace->Inventory_GetCapacity(); InventoryIndex++)
		{
			if (!iFace->Inventory_GetItem(InventoryIndex,item))
			{
				return InventoryIndex;
			}
		}
		return 4;
	}





	static bool IsInventoryFull(IExamInterface* iFace)
	{
		return !HasOpenSlot(iFace);

	}

	// Agent helpers
	static bool HasLowHealth(IExamInterface* iFace, float threshold)
	{
		return iFace->Agent_GetInfo().Health < threshold;


	}
	static bool IsHungry(IExamInterface* iFace, float hungerThreshold)
	{

		return iFace->Agent_GetInfo().Energy <= hungerThreshold;

	}

	// Enemy helpers
	static bool EnemyInRange(IExamInterface* iFace, float range) {

		auto enemies = iFace->GetEnemiesInFOV();
		Elite::Vector2 agentPos = iFace->Agent_GetInfo().Position;

		for (const auto& e : enemies)
			if ((e.Location - agentPos).Magnitude() < range)
				return true;
		return false;
	}
	static bool EnemiesAllOutOfRange(IExamInterface* iFace, float range) {

		auto enemies = iFace->GetEnemiesInFOV();
		Elite::Vector2 agentPos = iFace->Agent_GetInfo().Position;

		for (const auto& e : enemies)
			if ((e.Location - agentPos).Magnitude() <= range)
				return false;
		return true;
	}
};
