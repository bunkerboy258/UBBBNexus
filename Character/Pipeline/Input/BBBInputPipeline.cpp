
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Context/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Context/BBBCharacterWorldRuntimeData.h"

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

    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    InputProcessor.Update(*InputRawData, *InputData, *Config, DeltaSeconds);
}
