
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Execution/Executors/BBBEquipmentActionExecutor.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"

void FBBBEquipmentActionExecutor::Update(
    FBBBDecisionRuntimeData &DecisionData,
    FBBBCharacterEquipmentCommands &EquipmentCommands) const
{
    // 遍历当前帧请求并只消费已经批准的动作
    for (int32 Index = 0; Index < DecisionData.GetRequestCount(); ++Index)
    {
        FBBBCharacterActionRequest &Request = DecisionData.AccessRequestForExecution(Index);

        if (Request.GetArbitrationResult() != EBBBArbitrationResult::Approved)
        { continue; }

        if (Request.GetType() == EBBBCharacterActionType::Fire)
        {
            // 开火请求进入命令流后由装备系统决定实际执行时机
            EquipmentCommands.SubmitFire();
            Request.MarkConsumed();
        }

        if (Request.GetType() == EBBBCharacterActionType::Reload)
        {
            // 换弹请求进入命令流后由装备系统检查实际条件
            EquipmentCommands.SubmitReload();
            Request.MarkConsumed();
        }
    }
}
