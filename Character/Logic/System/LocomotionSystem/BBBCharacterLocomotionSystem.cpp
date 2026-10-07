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
    FBBBCharacterRuntimeData &InCharacterData
)
{
    Character = &InCharacter;
    Movement = &InMovement;
    RuntimeData = &InRuntimeData;
    ControlData = &InIntentData;
    Config = &InConfig;
    CharacterData = &InCharacterData;
    StrafeSpeedMapCurve = InConfig.StrafeSpeedMapCurve.LoadSynchronous();
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!Character || !Movement || !RuntimeData || !ControlData || !Config || !StrafeSpeedMapCurve || !CharacterData)
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
        CharacterData->Traversal.ReadTraversalState(),
        CharacterData->External.ReadNetworkIdentityState(),
        CharacterData->Animation.ReadAnimationFactState(),
        CharacterData->Life.ReadLifeState(),
        CharacterData->External.ReadWorldState().FrameDeltaSeconds,
        *CharacterData};
    LifeMovementProcessor.Update(Context);
    LocomotionProcessor.Update(Context);
}
