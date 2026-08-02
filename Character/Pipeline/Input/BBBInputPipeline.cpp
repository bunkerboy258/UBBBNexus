
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/BBBInputPipeline.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Input/BBBInputTypes.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"

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
    {
        return;
    }

    const float DeltaSeconds = WorldData->GetFrameDeltaSeconds();
    InputProcessor.Update(*InputRawData, *InputData, *Config, DeltaSeconds);
}
