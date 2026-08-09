#include "BBBWork/UBBBNexus/Character/System/AimSystem/BBBCharacterAimSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Aim/BBBAimConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterAimSystem::Initialize(
    APawn &InPawn,
    UCharacterMovementComponent &InMovement,
    FBBBAimRuntimeData &InAimData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBAimConfig &InAimConfig)
{
    Pawn = &InPawn;
    Movement = &InMovement;
    AimData = &InAimData;
    IntentData = &InIntentData;
    WorldData = &InWorldData;
    AimConfig = &InAimConfig;
}

void FBBBCharacterAimSystem::Update()
{
    if (!ensureMsgf(
        Pawn && Movement && AimData && IntentData && WorldData && AimConfig,
        TEXT("[UBBBC]Aim system update failed because dependencies are null")))
    {
        return;
    }

    FBBBAimRuntimeState State = AimData->GetState();

    AimStateProcessor.Update(*IntentData, State);

    if (State.bIsAiming)
    {
        AimTargetProcessor.Update(*Pawn, AimConfig->AimTargetDistance, State);
    }

    AimFacingProcessor.Update(
        *Pawn,
        *Movement,
        WorldData->GetFrameDeltaSeconds(),
        AimConfig->FacingInterpSpeed,
        State);

    AimData->CommitLocalState(State);
}
