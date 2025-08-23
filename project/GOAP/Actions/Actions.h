#pragma once

#include"../../stdafx.h"
#include"BaseAction.h"

class ConsumeSavedFood final : public BaseAction
{
public:
	ConsumeSavedFood();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:
	int m_MaxEnergy = 10;
};

class ConsumeSavedMedKit final : public BaseAction
{
public:
	ConsumeSavedMedKit();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:
	int m_MaxHealth = 10;
};


class GoToNearestSeenFood final : public BaseAction
{
public:
	GoToNearestSeenFood();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;
};


class GoToNearestSeenGun final : public BaseAction
{
public:
	GoToNearestSeenGun();

	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;
};

class GoToNearestSeenMedKit final : public BaseAction
{
public:
	GoToNearestSeenMedKit();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;
};



class MoveIntoHouse final : public BaseAction
{
public:
	MoveIntoHouse();
	bool Execute(float elapsedSec, SteeringPlugin_Output & steeringOutput, IExamInterface* iFace) override;

private:
	float m_PermittedDistanceFromHouseCenter{ 1 };
	float m_PermittedDistanceFromHouseCorners{ 4 };
};



class PickupFood final : public BaseAction
{
public:
	PickupFood();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:
	int m_FoodSlot = 4;
};


class PickupWeapon final : public BaseAction
{
public:
	PickupWeapon();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:

	int GetPistolSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace);
	int GetShotgunSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace);
	int m_MinAcceptedBulletCount{ 30 };

	UINT m_StartWeaponSlot{0};
	UINT m_EndWeaponSlot{1};
};


class PickupMedKit final : public BaseAction
{
public:
	PickupMedKit();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:

	int GetMedKitSelectedInventoryIndex(const ItemInfo& incomingItem, IExamInterface* iFace);

	int m_MaxHealth{ 10 };
	int m_StartMedKitInvSlot{2};
	int m_LastMedKitInvSlot{ 3 };
};



class RunFromPurge final : public BaseAction
{
public:
	RunFromPurge();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;
};





class Wander final : public BaseAction
{
public:
	Wander();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:

	void NewRandomWanderPosInSpiral(const WorldInfo& agentPos);
	void NewIncrementalWanderPosInSpiral(const WorldInfo& agentPos);
	bool NewWanderPosAwayFromPurges(const Elite::Vector2& closestPathPoint);


	Elite::Vector2 m_WanderPos;
	Elite::Vector2 m_WanderDir;
	float m_StartingDegreeFromCenterForWanderPos;
	float m_CompoundDegreeFromCenterForWanderPos;
	float m_MaxDegreeIncrease{ (2 * M_PI) / 20.f };//18 degrees
	float m_MinDegreeIncrease{ (2 * M_PI) / 10.f };//36 degrees
	const float m_MinDistanceFromPurge{ 10.f };
	const float m_OptimalDistanceFromPurge{ 50.0f };
	const float m_DistanceFromWanderPosToHaveNewWanderPos{ 8.0f };


	//Distance from world center
	const float m_MaxWanderDistanceMultiplier{ 0.8 };
	const float m_MinWanderDistanceMultiplier{ 0.4 };
	const float m_DeltaInMultiplierPerTime{ 0.03 };
	float m_CurrentDeltaInMultiplierPerTime{ m_DeltaInMultiplierPerTime };
	float m_CurrentMultiplier{ m_MinWanderDistanceMultiplier };
};



class ShootEnemyInView final : public BaseAction
{
public:
	ShootEnemyInView();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:
	float m_AcceptableRangeToShootIfShotGun{ 10 };
	float m_AcceptableRangeToShootIfPistol{ 10 };
};



class LeaveHouse final : public BaseAction
{
public:
	LeaveHouse();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;
};



class SprintAwayFromEnemy final : public BaseAction
{
public:
	SprintAwayFromEnemy();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

};


class TurnReallyFast final : public BaseAction
{
public:
	TurnReallyFast();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


