
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Processors/BBBLocomotionIntentProcessor.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/States/BBBIntentStates.h"

void FBBBLocomotionIntentProcessor::Update(
    const FBBBInputRuntimeData &InputData,
    const FBBBIntentRuntimeData &IntentData,
    const FBBBCharacterLocomotionConfig &Config,
    float DeltaSeconds,
    FBBBCharacterIntentState &Intent) const
{
    const FBBBProcessedInputFrame &Input = InputData.GetProcessedInput();

    //上一帧保留的平滑值
    FVector2D Smoothed = IntentData.GetSmoothedMoveInput();

    Smoothed.X = FMath::FInterpTo(Smoothed.X, Input.MoveInput.X, DeltaSeconds, 1.0f / FMath::Max(Config.MoveInputXSmoothTime, KINDA_SMALL_NUMBER));
    Smoothed.Y = FMath::FInterpTo(Smoothed.Y, Input.MoveInput.Y, DeltaSeconds, 1.0f / FMath::Max(Config.MoveInputYSmoothTime, KINDA_SMALL_NUMBER));

    Intent.MoveInput = Input.MoveInput;
    Intent.LookDelta = Input.LookDelta;
    Intent.SmoothedMoveInput = Smoothed;
    Intent.bHasMoveInput = Input.bHasMoveInput;
    Intent.bWantsSprint = Input.bSprintHeld;
    
}
