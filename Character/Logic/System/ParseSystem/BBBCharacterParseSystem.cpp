#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"

void FBBBCharacterParseSystem::Initialize(FBBBCharacterRuntimeData &InData)
{
    Data = &InData;
}

void FBBBCharacterParseSystem::Update() const
{
    if (!Data)
    {
        return;
    }

    FBBBCharacterInputContext Context{
        Data->Animation.AnimationMontageState,
        Data->Aim.AimState,
        Data->Locomotion.LocomotionState,
        Data->Parse.ControlState,
        Data->Parse.CameraState.PendingInput};
    InputProcessor.Update(Data->Parse.InputState, Context);
}
