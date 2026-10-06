#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/BBBCharacterLocomotionDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterLocomotionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InCharacter,
    UCharacterMovementComponent &InMovement,
    FBBBCharacterLocomotionDomainState &InRuntimeData,
    const FBBBCharacterControlState &InIntentData,
    const FBBBCharacterLocomotionConfig &InConfig,
    const FBBBTraversalConfig &InTraversalConfig,
    const FBBBCharacterRuntimeData &InCharacterData,
    UMotionWarpingComponent &InWarping)
{
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    ControlData = &InIntentData;
    Config = &InConfig;
    TraversalConfig = &InTraversalConfig;
    CharacterData = &InCharacterData;
    Warping = &InWarping;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!Character || !Movement || !RuntimeData || !ControlData || !Config || !StrafeSpeedMapCurve || !TraversalConfig || !CharacterData || !Warping)
    {
        return;
    }

    FBBBCharacterLocomotionUpdateContext Context{
        *Character,
        *Movement,
        RuntimeData->LocomotionState,
        *ControlData,
        *Config,
        *StrafeSpeedMapCurve,
        RuntimeData->TraversalState,
        *TraversalConfig,
        CharacterData->External.ReadWorldState(),
        CharacterData->External.ReadNetworkIdentityState(),
        CharacterData->Animation.ReadAnimationFactState(),
        CharacterData->Animation.ReadAnimationMontageState(),
        *Warping};
    ProbeProcessor.Update(Context);
    LifeProcessor.Update(Context);
    WarpProcessor.Update(Context);
    LocomotionProcessor.Update(Context);
}
