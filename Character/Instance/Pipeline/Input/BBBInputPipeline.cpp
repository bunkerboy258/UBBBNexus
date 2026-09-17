
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
    if (!ensureMsgf(WorldData && InputRawData && InputData && Config, TEXT("[UBBBC]Input pipeline update failed because dependencies are null")))
    { return; }

    InputData->ReloadInputs = MoveTemp(InputData->PendingReloadInputs);
    InputData->PendingReloadInputs.Reset();

    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    InputProcessor.Update(*InputRawData, *InputData, *Config, DeltaSeconds);
}
