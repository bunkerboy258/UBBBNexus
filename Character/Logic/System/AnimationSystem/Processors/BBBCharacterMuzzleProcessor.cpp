#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterMuzzleProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterMuzzleProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    FBBBCharacterAnimationFactState &State = Context.RuntimeData.Animation.AnimationFactState;
    State.MuzzleTransformHandRSpace = FTransform::Identity;
    State.bHasMuzzle = false;

    const ABBBEquipment *Equipment =
        Context.RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance;
    if (!IsValid(Equipment))
    {
        return;
    }

    FTransform MuzzleWorld;
    if (!Equipment->TryGetMuzzleTransform(MuzzleWorld))
    {
        return;
    }

    const FName HandBoneName(TEXT("hand_r"));
    if (Context.CharacterMesh.GetBoneIndex(HandBoneName) == INDEX_NONE)
    {
        return;
    }

    const FTransform HandWorld = Context.CharacterMesh.GetSocketTransform(HandBoneName, RTS_World);
    const FTransform MuzzleHandRSpace = MuzzleWorld.GetRelativeTransform(HandWorld);
    if (!MuzzleHandRSpace.IsValid())
    {
        return;
    }

    State.MuzzleTransformHandRSpace = MuzzleHandRSpace;
    State.bHasMuzzle = true;
}
