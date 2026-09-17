
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Arbitration/BBBArbitrationPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"

void FBBBArbitrationPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    const FBBBCharacterEquipmentState &InEquipmentState)
{
    DecisionData = &InDecisionData;
    EquipmentState = &InEquipmentState;
}

void FBBBArbitrationPipeline::Update() const
{
    // 仲裁阶段需要完整的请求数据和装备状态
    if (!ensureMsgf(DecisionData && EquipmentState, TEXT("[UBBBC]Arbitration pipeline update failed because dependencies are null")))
    { return; }

    // 先根据装备状态排除当前不可用的请求
    EquipmentStateArbitrator.Update(*DecisionData, *EquipmentState);
    // 再处理动作领域之间的互斥关系
    ActionDomainArbitrator.Update(*DecisionData);
    // 最后在剩余请求中应用优先级规则
    ActionPriorityArbitrator.Update(*DecisionData);
}
