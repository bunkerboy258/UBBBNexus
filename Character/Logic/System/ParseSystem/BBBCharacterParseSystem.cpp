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
        Data->Equipment.EquipmentSelectionState,
        Data->Item.ItemOperationState,
        Data->External.ReadNetworkIdentityState().bIsMirror,
        Data->Parse.CameraState,
        Data->Animation.AimImpulseState,
        Data->Animation.ReadAnimationFactState(),
        Data->Traversal.TraversalState,
        Data->Equipment.EquipmentAnimationInputState,
        Data->Life.LifeInputState,
        Data->Life.ReadLifeState(),
        Data->Equipment.EquipmentUseInputState,
        Data->Network.DamageInboxState,
        Data->Life.RescueInputState,
        Data->Life.ReadRescueState(),
        Data->Network.RescueInboxState,
        Data->Appearance.InputState};
    InputProcessor.Update(Data->Parse.InputState, Context);
}
