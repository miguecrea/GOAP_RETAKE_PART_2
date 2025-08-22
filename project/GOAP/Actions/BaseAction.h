#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Exam_HelperStructs.h"


class IExamInterface;
class BaseWorldState;

class BaseAction
{
public:

    BaseAction() = default;
    virtual ~BaseAction();

    BaseAction(const BaseAction& other) = delete;
    BaseAction& operator=(const BaseAction& other) = delete;

    BaseAction(BaseAction&& other) noexcept;
    BaseAction& operator=(BaseAction&& other) noexcept;


    virtual bool Execute(float elapsedSec, SteeringPlugin_Output& steeringOutput, IExamInterface* iFace) = 0;

    float GetWeight() const;
    int GetGraphNodeIndex() const;
    const std::string& GetName() const;
    const std::vector<std::unique_ptr<BaseWorldState>>& GetPreconditions() const;
    const std::vector<std::unique_ptr<BaseWorldState>>& GetEffects() const;

    void SetGraphNodeIndex(int idx);

protected:
    void SetWeight(float weight);
    void SetName(const std::string& name);

    void AddPrecondition(std::unique_ptr<BaseWorldState> state);
    void AddEffect(std::unique_ptr<BaseWorldState> state);

    std::vector<std::unique_ptr<BaseWorldState>> m_Preconditions;
    std::vector<std::unique_ptr<BaseWorldState>> m_Effects;

private:
    std::string m_Name;
    float m_Weight;
    int m_GraphNodeIdx;
};

