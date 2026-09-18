#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBPlayerCameraInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return !Impulse.ContainsNaN() && FMath::IsFinite(RecoverySpeed) && RecoverySpeed > 0.0f;
}

void FBBBPlayerCameraInput::BeginFrame(FBBBCharacterRuntimeData &Data)
{
    Data.CameraContributions.Reset();
}

void FBBBPlayerCameraInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.CameraContributions.Add(*this);
}
