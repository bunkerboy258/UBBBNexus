
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Planners/BBBCharacterActionPlanner.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"

void FBBBCharacterActionPlanner::Update(
    const FBBBIntentRuntimeData &Intent,
    FBBBDecisionRuntimeData &DecisionData) const
{
    // 规划器只生成候选请求最终结果由后续仲裁和执行阶段决定
    //装备切换占用全身动作域 并拥有最高优先级
    if (Intent.GetRequestedEquipSlot() != INDEX_NONE)
    {
        DecisionData.AddRequest(
            FBBBCharacterActionRequest(
                EBBBCharacterActionType::Equip,
                EBBBCharacterActionDomain::FullBody,
                60,
                Intent.GetRequestedEquipSlot()));
    }

    //换弹占用上半身动作域 并优先于开火
    if (Intent.WantsReload())
    {
        DecisionData.AddRequest(
            FBBBCharacterActionRequest(
                EBBBCharacterActionType::Reload,
                EBBBCharacterActionDomain::UpperBody,
                50));
    }

    //持续开火使用上半身动作域的较低优先级
    if (Intent.WantsFire())
    {
        DecisionData.AddRequest(
            FBBBCharacterActionRequest(
                EBBBCharacterActionType::Fire,
                EBBBCharacterActionDomain::UpperBody,
                40));
    }
}
