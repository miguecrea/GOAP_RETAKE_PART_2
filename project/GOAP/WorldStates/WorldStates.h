#pragma once

#include"BaseWorldState.h"
#include"Exam_HelperStructs.h"




class HasSavedUpFood : public BaseWorldState {
public:
	explicit HasSavedUpFood(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasSavedUpFood";
	}
	void Update(float elapsedSec, IExamInterface* iFace) override;

};


class HasSavedUpMedKits : public BaseWorldState
{
public:
	explicit HasSavedUpMedKits(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HasSavedUpMedKits";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;

};

class IsLoadedWithMedKits : public BaseWorldState {
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


class IExamInterface;
class HasSavedWeaponsWithAcceptableAmmo : public BaseWorldState {
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

class IExamInterface;
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


class ThereAreHousesToVisit : public BaseWorldState {
public:
	explicit ThereAreHousesToVisit(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "HouseInViewState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class IsHungry : public BaseWorldState
{
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
		:BaseWorldState(predicate)
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


class KnowsFoodLocation : public BaseWorldState {
public:
	explicit KnowsFoodLocation(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "KnowsFoodLocation";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class KnowsMedKitLocation : public BaseWorldState {
public:
	explicit KnowsMedKitLocation(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "KnowsMedKitLocation";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class KnowsWeaponLocation : public BaseWorldState 
{
public:
	explicit KnowsWeaponLocation(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "KnowsWeaponLocation";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class NextToFood : public BaseWorldState
{
public:
	explicit NextToFood(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "NextToFood";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class NextToMedKit : public BaseWorldState {
public:
	explicit NextToMedKit(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "NextToMedKit";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};


class NextToWeapon : public BaseWorldState {
public:
	explicit NextToWeapon(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "NextToWeapon";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};

class RecentlyBittenState : public BaseWorldState {
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

class ZombieInViewState : public BaseWorldState {
public:
	explicit ZombieInViewState(bool predicate)
		: BaseWorldState(predicate)
	{
		m_Name = "ZombieInViewState";
	}

	void Update(float elapsedSec, IExamInterface* iFace) override;
};
