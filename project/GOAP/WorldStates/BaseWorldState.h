#pragma once
#include <string>

class IExamInterface;

class BaseWorldState
{
public:
	virtual ~BaseWorldState() = default;

	BaseWorldState() = delete;
	BaseWorldState(bool predicate);

	virtual void Update(float elapsedSec, IExamInterface * iFace) = 0;
	bool m_Predicate;
	std::string m_Name;
};

