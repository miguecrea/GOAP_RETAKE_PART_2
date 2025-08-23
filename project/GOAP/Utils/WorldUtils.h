


class IExamInterface;

#include"Exam_HelperStructs.h"

class WorldUtils
{
public:

	static const UINT m_StartWeaponSlot{ 0 };
	static const UINT m_EndWeaponSlot{ 1 };
	static const int FoodSloT = 4;
	static const int  m_MinAcceptedBulletCount{ 30 };

	static const int m_StartMedKitInvSlot{ 2 };
	static const int m_LastMedKitInvSlot{ 3 };


	static int GetMedKitSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace)
	{

		ItemInfo currentItem{};

		for (UINT i = m_StartMedKitInvSlot; i <= m_LastMedKitInvSlot; i++)
		{
			if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == eItemType::MEDKIT)
			{ //already a med kit there 
				continue;
			}
			else
			{
				iFace->Inventory_RemoveItem(i);
				return i;
			}

		}
		return -1;
	}

	static bool GoToNearestSeenItem(IExamInterface* iFace, eItemType type,SteeringPlugin_Output & steeringOutput, eItemType type2 = eItemType::_LAST)
	{

		std::vector<ItemInfo> seenItems = WorldMemory::Instance()->GetAllItemsInMemory();
		std::vector<PurgeZoneInfo> seenPurges = WorldMemory::Instance()->GetAllSeenPurges();

		if (seenItems.empty())
		{
			return true;
		}

		auto agentInfo = iFace->Agent_GetInfo();
		float currentDistance = 0;
		float nearestDistance = FLT_MAX;
		bool itemInPurgeZone = false;
		bool chosenItemInPurgeZone = false;
		Elite::Vector2 target{};

		for (size_t i = 0; i < seenItems.size(); i++)
		{



			if (seenItems[i].Type == type || seenItems[i].Type == type2)
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


	static bool InventoryContains(IExamInterface* iFace, eItemType type, int minValue = 0)
	{
		ItemInfo item{};
		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, item) && item.Type == type)
				return true;
		}
		return false;
	}

	static int GetShotgunSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace)
	{



		ItemInfo currentItem{};

		for (UINT i = m_StartWeaponSlot; i <= m_EndWeaponSlot; i++)
		{
			if (iFace->Inventory_GetItem(i, currentItem))
			{
				if (currentItem.Value < m_MinAcceptedBulletCount && incomingItem.Value > currentItem.Value)
				{
					iFace->Inventory_RemoveItem(i);
					return i;
				}
			}
			else
			{
				// nothing on this slot 
				return i;
			}

		}
		return -1;



	}


	static int GetPistolSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace)
	{

		ItemInfo currentItem{};

		for (UINT i = m_StartWeaponSlot; i <= m_EndWeaponSlot; i++)
		{
			if (iFace->Inventory_GetItem(i, currentItem))
			{
				if (currentItem.Type != eItemType::SHOTGUN && currentItem.Value < m_MinAcceptedBulletCount && incomingItem.Value > currentItem.Value)
				{
					iFace->Inventory_RemoveItem(i);
					return i;
				}
			}
			else
			{
				return i;
			}

		}
		return -1;
	}


	static bool PickUpItem(IExamInterface* iFace)
	{

		std::vector<ItemInfo> itemInfos = iFace->GetItemsInFOV();

		ItemInfo currentItem{};
		bool pickedUp = false;
		int selectedIndex{ 0 };

		for (const auto& itemInfo : itemInfos)
		{
			if ((itemInfo.Location - iFace->Agent_GetInfo().Position).Magnitude() < iFace->Agent_GetInfo().GrabRange)
			{

				switch (itemInfo.Type)
				{
				case eItemType::FOOD:

					//if my food slot is empty  grab it 
					if (!iFace->Inventory_GetItem(FoodSloT, currentItem))
					{
						iFace->GrabItem(itemInfo);
						iFace->Inventory_AddItem(FoodSloT, itemInfo);
						pickedUp = true;
					}

					break;

				case eItemType::MEDKIT:

					selectedIndex = GetMedKitSelectedInventoryIndex(itemInfo, iFace);

					if (selectedIndex >= 0) //was picked is succesf
					{

						pickedUp = true;

						iFace->GrabItem(itemInfo);

						iFace->Inventory_AddItem(selectedIndex, itemInfo);

						break;

				case eItemType::PISTOL:

					selectedIndex = GetPistolSelectedInventoryIndex(itemInfo, iFace);

					if (selectedIndex >= 0) {

						pickedUp = true;
						iFace->GrabItem(itemInfo);
						iFace->Inventory_AddItem(selectedIndex, itemInfo);
					}


					break;


				case eItemType::SHOTGUN:

					selectedIndex = GetShotgunSelectedInventoryIndex(itemInfo, iFace);

					if (selectedIndex >= 0) {

						pickedUp = true;
						iFace->GrabItem(itemInfo);
						iFace->Inventory_AddItem(selectedIndex, itemInfo);
					}


					break;

				default:
					break;
					}

					if (pickedUp)
					{
						WorldMemory::Instance()->RemoveItemFromMemory(itemInfo);
					}



				}
			}
			return false;




		}
	}



	static bool ConsumeItem(IExamInterface* iFace, eItemType type)
	{

		ItemInfo currentItem{};
		auto agentInfo = iFace->Agent_GetInfo();


		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == type)
			{
				iFace->Inventory_UseItem(i);
				iFace->Inventory_RemoveItem(i);

				switch (type)
				{
				default:
				case eItemType::MEDKIT:
					agentInfo.Health += currentItem.Value;
					IsAttributeHigherThanMax(agentInfo.Health);
					break;
				case eItemType::FOOD:
					agentInfo.Energy += currentItem.Value;
					IsAttributeHigherThanMax(agentInfo.Energy);
					break;
				}

				return false;

			}
		}


		return false;
	}




	static bool IsAttributeHigherThanMax(float AgentAttribute)
	{
		return AgentAttribute >= 10.f;

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


	static bool HasNumberOfItemsWithAcceptableValue(IExamInterface* iFace, const std::vector<eItemType>& types, int AcceptableItems, int acceptableValue)
	{

		ItemInfo itemReference;
		int count = 0;

		for (size_t i = 0; i < iFace->Inventory_GetCapacity(); ++i)
		{
			if (!iFace->Inventory_GetItem(i, itemReference))
				continue;

			if (std::find(types.begin(), types.end(), itemReference.Type) != types.end() &&
				itemReference.Value >= acceptableValue)
			{
				++count;

				if (count >= AcceptableItems)
					return true;
			}
		}

		return false;
	}


	static bool NextToItem(IExamInterface* iFace, const std::vector<eItemType>& type)
	{

		std::vector<ItemInfo> itemInfo = iFace->GetItemsInFOV();

		for (const ItemInfo& info : itemInfo)
		{
			for (const eItemType& Item : type)
			{
				if (info.Type == Item)
				{
					if ((info.Location - iFace->Agent_GetInfo().Position).Magnitude() <= iFace->Agent_GetInfo().GrabRange)
					{
						return true;
					}
				}

			}
		}

		return false;


	}


	static bool KnowsItemLocation(IExamInterface* iFace, const std::vector<eItemType>& type)
	{
		std::vector<ItemInfo> itemInfo = WorldMemory::Instance()->GetAllItemsInMemory();


		for (const ItemInfo& info : itemInfo)
		{
			for (const eItemType& Item : type)
			{
				if (info.Type == Item)
				{
					return true;
				}

			}
		}

		return false;

	}



	static float AddValueOfItem(IExamInterface* iFace, eItemType type)
	{


		ItemInfo currentItem{};

		float TotalValue{ 0.f };

		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (iFace->Inventory_GetItem(i, currentItem) && currentItem.Type == type)
			{
				TotalValue += currentItem.Value;
			}
		}

		return TotalValue;


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


	static bool HasOpenSlot(IExamInterface* iFace)
	{

		ItemInfo item{};

		for (UINT i = 0; i < iFace->Inventory_GetCapacity(); i++)
		{
			if (!iFace->Inventory_GetItem(i, item))
				return true;
		}
		return false;
	}

	static int GetFirstOpenSlot(IExamInterface* iFace)
	{

		ItemInfo item{};

		for (UINT InventoryIndex = 0; InventoryIndex < iFace->Inventory_GetCapacity(); InventoryIndex++)
		{
			if (!iFace->Inventory_GetItem(InventoryIndex, item))
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
