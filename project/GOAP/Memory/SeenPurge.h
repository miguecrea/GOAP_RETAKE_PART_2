#pragma once
#include "../../stdafx.h"
#include <Exam_HelperStructs.h>

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

