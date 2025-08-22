#include "../../stdafx.h"
#include "VisitedHouse.h"

VisitedHouse::VisitedHouse(const HouseInfo& hi)
{
	m_HouseInfo = hi;
}

bool VisitedHouse::HasBeenForgotten(float elapsedSec)
{
	m_SecondsSinceVisit += elapsedSec;
	if (m_SecondsSinceVisit > m_SecondsTillRefresh)
		return true;
	else
		return false;
}