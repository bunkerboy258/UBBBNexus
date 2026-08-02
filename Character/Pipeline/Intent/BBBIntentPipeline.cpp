
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipeline.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/BBBIntentPipelineTypes.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"

void FBBBIntentPipeline::Initialize(
    FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBInputRuntimeData &InInputData,
    const FBBBCharacterLocomotionConfig &InLocomotionConfig)
{
    IntentData = &InIntentData;
    WorldData = &InWorldData;
    InputData = &InInputData;
    LocomotionConfig = &InLocomotionConfig;
}

void FBBBIntentPipeline::Update() const
{
    if (!ensureMsgf(WorldData && InputData && IntentData && LocomotionConfig, TEXT("[UBBBC]Intent pipeline update failed because dependencies are null")))
    {
        return;
    }

    FBBBCharacterIntentState Intent;

    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();

    //复制基础输入(为了保持数据流向一致)
    InputProcessor.Update(*InputData, Intent);

    //计算平滑移动方向与冲刺意图
    LocomotionProcessor.Update(*InputData, *IntentData, *LocomotionConfig, DeltaSeconds, Intent);

    //根据瞄准开火与宽限状态合成瞄准意图
    AimProcessor.Update(*InputData, Intent);

    //把开火与换弹输入转换为装备操作意图
    EquipmentProcessor.Update(*InputData, Intent);

    //按槽位按键优先级生成装备切换意图
    ItemProcessor.Update(*InputData, Intent);

    //所有处理器结束后一次性发布完整意图快照
    IntentData->CommitFrame(Intent.ToRuntimeData());
}
