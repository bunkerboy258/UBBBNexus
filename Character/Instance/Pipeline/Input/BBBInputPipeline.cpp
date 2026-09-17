
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Input/BBBInputConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/States/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/Definition/BBBCharacterWorldRuntimeData.h"

void FBBBInputPipeline::Initialize(
    FBBBInputRuntimeData &InInputData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBInputRawData &InInputRawData,
    const FBBBInputPipelineConfig &InConfig)
{
    InputData = &InInputData;
    WorldData = &InWorldData;
    InputRawData = &InInputRawData;
    Config = &InConfig;
}

void FBBBInputPipeline::Update() const
{
    // 输入帧生成需要原始输入运行时数据世界时间和输入配置
    if (!ensureMsgf(WorldData && InputRawData && InputData && Config, TEXT("[UBBBC]Input pipeline update failed because dependencies are null")))
    { return; }

    // 将动画通知产生的换弹输入移交给本帧输入处理
    InputData->ReloadInputs = MoveTemp(InputData->PendingReloadInputs);
    // 清空待处理队列使下一帧只接收新的动画通知
    InputData->PendingReloadInputs.Reset();

    // 缓冲计时器统一使用当前世界帧间隔衰减
    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    // 处理器负责生成后续意图读取的稳定输入
    InputProcessor.Update(*InputRawData, *InputData, *Config, DeltaSeconds);
}
