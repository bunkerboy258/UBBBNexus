#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/BBBCharacterTraversalSystem.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/Context/BBBCharacterTraversalUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterTraversalSystem::Initialize(ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InData, const FBBBTraversalConfig &InConfig, UMotionWarpingComponent &InWarping)
{
    Character = &InCharacter;
    Data = &InData;
    Config = &InConfig;
    Warping = &InWarping;
}

void FBBBCharacterTraversalSystem::Update()
{
    if (!Character || !Data || !Config || !Warping || !Character->GetCharacterMovement())
    {
        return;
    }

    FBBBCharacterTraversalUpdateContext Context{
        *Character,
        *Character->GetCharacterMovement(),
        Data->Traversal.TraversalState,
        Data->Parse.ReadControlState(),
        *Config,
        Data->External.ReadWorldState(),
        Data->External.ReadNetworkIdentityState(),
        Data->Animation.ReadAnimationMontageState(),
        Data->Locomotion.ReadLocomotionState(),
        Data->Animation.ReadTraversalAnimationState(),
        *Warping,
        Data->Life.ReadLifeState()};

    LifeProcessor.Update(Context);
    ProbeProcessor.Update(Context);
    WarpProcessor.Update(Context);
}
