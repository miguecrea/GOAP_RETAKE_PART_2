#pragma once
#include <Exam_HelperStructs.h>

class VisitedHouse
{
public:
	VisitedHouse(const HouseInfo& hi);
	bool HasBeenForgotten(float elapsedSec);
	const HouseInfo& GetHouseInfo() const { return m_HouseInfo; }
private:
	HouseInfo m_HouseInfo;
	float m_SecondsSinceVisit = 0;
	float m_SecondsTillRefresh = 2000.f;
};

struct UnvisitedHouse
{
	UnvisitedHouse(const HouseInfo& hi) : HouseInfo(hi)
	{
		Elite::Vector2 size = hi.Size * SizeMultiplier;
		Elite::Vector2 flip = hi.Size * SizeMultiplier;
		flip.y = -flip.y;

		PointsToVisit.emplace_back(hi.Center - flip / 2.f);//top left
		PointsToVisit.emplace_back(hi.Center + size / 2.f);//top right
		PointsToVisit.emplace_back(hi.Center + flip / 2.f);//bottom right
		PointsToVisit.emplace_back(hi.Center - size / 2.f);//bottom left
		PointsToVisit.emplace_back(hi.Center);
	}
	UnvisitedHouse()
	{
	}
	std::vector<Elite::Vector2> PointsToVisit;
	HouseInfo HouseInfo;
	float SizeMultiplier = 0.5f;
	int MaxSize = 5;
};

