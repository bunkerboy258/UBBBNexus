#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRescueMovementProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterRescueMovementProcessor::Update(FBBBCharacterLocomotionUpdateContext &Context) const
{
    const auto &Rescue = Context.Data.Life.ReadRescueState();
    if ((!Rescue.bHelping && !Rescue.bReceiving) || Context.Life.Phase == EBBBCharacterLifePhase::Dead)
    {
        return;
    }
    Context.Character.StopJumping();
    Context.Character.ConsumeMovementInputVector();
    Context.Movement.StopMovementImmediately();
    Context.Movement.ClearAccumulatedForces();
    Context.LocomotionState.bRun = false;
    if (!Context.Execution.bIsMirror && Rescue.bHelping && Rescue.Partner.IsValid())
    {
        const FVector Direction = Rescue.Partner->GetActorLocation() - Context.Character.GetActorLocation();
        if (!Direction.IsNearlyZero())
        {
            Context.Character.SetActorRotation(FRotator(0, Direction.Rotation().Yaw, 0));
        }
    }
}
