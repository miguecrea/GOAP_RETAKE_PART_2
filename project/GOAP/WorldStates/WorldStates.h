#pragma once

#include"BaseWorldState.h"
#include"Exam_HelperStructs.h"

class HasSavedUpItem : public BaseWorldState
{
public:

	explicit HasSavedUpItem(bool predicate, const eItemType& ItemType)
		: BaseWorldState(predicate), m_ItemType(ItemType)
	{

		switch (m_ItemType)
		{
		case eItemType::FOOD:
			m_Name = "HasSavedUpFood";
			break;
		case eItemType::MEDKIT:

			m_Name = "HasSavedUpMedKits";
			break;
		default:
			break;

		}
	}

	eItemType m_ItemType;

	void Update(float elapsedSec, IExamInterface* iFace) override;

};


class IsLoadedWithMedKits : public BaseWorldState    /////Same as above
{
public:
	explicit IsLoadedWithMedKits(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsLoadedWithMedKits";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	int m_AcceptableAmountOfMedKits{ 2 };
};

class HasSavedWeaponsWithAcceptableAmmo : public BaseWorldState //same as above 
{
public:
	explicit HasSavedWeaponsWithAcceptableAmmo(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasSavedWeaponsWithAcceptableAmmo";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	int m_AcceptableAmmo = 100;
	int m_NumberOfAcceptableWeapons = 2;
};


class HasVisitedAllSeenHouses : public BaseWorldState {
public:
	explicit HasVisitedAllSeenHouses(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasVisitedAllSeenHouses";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
};


class HasWeaponState : public BaseWorldState {
public:
	explicit HasWeaponState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasWeaponState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class SafeFromEnemy : public BaseWorldState {
public:
	explicit SafeFromEnemy(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "SafeFromEnemy";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};



class HouseInViewState : public BaseWorldState {
public:
	explicit HouseInViewState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HouseInViewState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class IsHungry : public BaseWorldState {
public:
	explicit IsHungry(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsHungry";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	float m_HungerThresholdIfNoFoodInInventory = 5.f;
	float m_MaxEnergyCapacity = 10.f;
	float m_LowestHungerThreshold = 1.f;
};


class IsHurtState : public BaseWorldState {
public:
	explicit IsHurtState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsHurtState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	float m_HealthThreshold = 7.f;
};

class IsInHouseState : public BaseWorldState {
public:
	explicit IsInHouseState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsInHouseState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class IsInPurgeZoneState : public BaseWorldState {
public:
	explicit IsInPurgeZoneState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsInPurgeZoneState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class KnowsItemLocation : public BaseWorldState
{
public:
	explicit KnowsItemLocation(bool predicate, const eItemType& ItemType)
		: BaseWorldState(predicate), m_Item{ ItemType }
	{
		switch (m_Item)
		{
		case eItemType::FOOD:

			m_Name = "KnowsFoodLocation";

			break;

		case eItemType::MEDKIT:

			m_Name = "KnowsMedKitLocation";

			break;

		case eItemType::PISTOL:
		case eItemType::SHOTGUN:

			m_Name = "KnowsWeaponLocation";
			break;
		default:
			break;
		}
	}

	eItemType m_Item;

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class NextToItem : public BaseWorldState
{
public:
	explicit NextToItem(bool predicate, const eItemType& ItemType)
		: BaseWorldState(predicate), m_Item{ ItemType }
	{
		switch (m_Item)
		{
		case eItemType::FOOD:

			m_Name = "NextToFood";

			break;

		case eItemType::MEDKIT:

			m_Name = "NextToMedKit";
			break;

		case eItemType::PISTOL:
		case eItemType::SHOTGUN:

			m_Name = "NextToWeapon";
			break;
		default:
			break;
		}
	}


	eItemType m_Item;


	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class RecentlyBittenState : public BaseWorldState
{
public:
	explicit RecentlyBittenState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "RecentlyBittenState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	float m_DefaultGracePeriod = 2.f;
	float m_GracePeriod = 0.f;
};


class IsInventoryFull : public BaseWorldState
{
public:
	explicit IsInventoryFull(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsInventoryFull";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:
	float m_DefaultGracePeriod = 2.f;
	float m_GracePeriod = 0.f;
};


class IsLowOnAmmo : public BaseWorldState
{
public:
	explicit IsLowOnAmmo(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "LowOnAmmo";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:

	int m_AmmoThreshold = 6;
};

class IsNearEnemy : public BaseWorldState
{
public:
	explicit IsNearEnemy(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "IsNearEnemy";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

private:

};

class ZombieInViewState : public BaseWorldState
{
public:
	explicit ZombieInViewState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "ZombieInViewState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class HasOpenInventorySlot : public BaseWorldState
{
public:
	explicit HasOpenInventorySlot(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasOpenInventorySlot";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};




