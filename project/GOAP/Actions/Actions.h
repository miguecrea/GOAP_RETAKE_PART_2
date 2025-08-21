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

class EvadeEnemy final : public BaseAction
{
public:

	EvadeEnemy();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

private:
};

class GoToNearestSeenItem final : public BaseAction
{
public:
	GoToNearestSeenItem(const eItemType& Item);
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;

	eItemType m_DesiredItem;
};

class Wander final : public BaseAction
{
public:
	Wander();
	bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) override;


private:
	// Wander circle params
	float m_CircleDistance = 15.f;   // distance ahead of agent
	float m_CircleRadius = 8.f;      // radius of wander circle
	float m_AngleChange = 0.5f;      // max change in wander angle per update
	float m_WanderAngle = 0.f;

	// Purge avoidance params
	float m_MinDistanceFromPurge = 15.f;
	float m_OptimalDistanceFromPurge = 30.f;
};

