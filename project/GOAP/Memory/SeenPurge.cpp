#include "../../stdafx.h"
#include "SeenPurge.h"

SeenPurge::SeenPurge(const PurgeZoneInfo& zi)
{
	m_PurgeInfo = zi;
}

bool SeenPurge::ZoneIsSafeAgain(float elapsedSec)
{
	m_SecondsSinceSeen += elapsedSec;
	if (m_SecondsSinceSeen > m_PurgeZoneMaxTimer)
	{
		return true;
	}
	return false;
}
