#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/Processors/BBBCharacterMuzzleProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterMuzzleProcessor::Update(FBBBCharacterAnimationUpdateContext &Context) const
{
    FBBBCharacterAnimationFactState &State = Context.RuntimeData.Animation.AnimationFactState;
    State.MuzzleTransformHandRSpace = FTransform::Identity;

    const ABBBEquipment *Equipment =
        Context.RuntimeData.Equipment.ReadEquipmentSelectionState().ActiveMainHandInstance;
    if (!IsValid(Equipment))
    {
        return;
    }

    const USkeletalMeshComponent *EquipmentMesh = Equipment->GetEquipmentSkeletalMesh();
    if (!EquipmentMesh)
    {
        return;
    }

    const FName MuzzleSocketName(TEXT("Muzzle"));
    const FName HandBoneName(TEXT("hand_r"));
    if (!ensureMsgf(
        EquipmentMesh->DoesSocketExist(MuzzleSocketName)
            && Context.CharacterMesh.GetBoneIndex(HandBoneName) != INDEX_NONE,
        TEXT("枪口动画事实获取失败 装备 %s 需要 Muzzle Socket 角色需要 hand_r 骨骼"),
        *GetNameSafe(Equipment)))
    {
        return;
    }

    const FTransform MuzzleWorld = EquipmentMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
    const FTransform HandWorld = Context.CharacterMesh.GetSocketTransform(HandBoneName, RTS_World);
    const FTransform MuzzleHandRSpace = MuzzleWorld.GetRelativeTransform(HandWorld);
    if (!ensureMsgf(
        MuzzleHandRSpace.IsValid(),
        TEXT("枪口动画事实包含无效变换 装备 %s"),
        *GetNameSafe(Equipment)))
    {
        return;
    }

    State.MuzzleTransformHandRSpace = MuzzleHandRSpace;
}
