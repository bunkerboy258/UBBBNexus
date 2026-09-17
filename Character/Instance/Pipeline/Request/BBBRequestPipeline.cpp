
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/BBBRequestPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"

void FBBBRequestPipeline::Initialize(
    FBBBDecisionRuntimeData &InDecisionData,
    const FBBBIntentRuntimeData &InIntentData)
{
    DecisionData = &InDecisionData;
    IntentData = &InIntentData;
}

void FBBBRequestPipeline::Update() const
{
    // 请求生成需要稳定意图和可写的仲裁数据
    if (!ensureMsgf(IntentData && DecisionData, TEXT("[UBBBC]Request pipeline update failed because dependencies are null")))
    { return; }

    // 将本帧意图展开为等待仲裁的动作请求
    ActionPlanner.Update(*IntentData, *DecisionData);
    // 将换弹通知传递给仲裁阶段验证动作序号
    DecisionData->ReloadInputs = IntentData->ReloadInputs;
}
