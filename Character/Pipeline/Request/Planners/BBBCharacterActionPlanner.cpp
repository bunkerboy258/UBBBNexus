
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Planners/BBBCharacterActionPlanner.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Context/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Context/BBBIntentRuntimeData.h"

void FBBBCharacterActionPlanner::Update(
    const FBBBIntentRuntimeData &Intent,
    FBBBDecisionRuntimeData &DecisionData) const
{
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
