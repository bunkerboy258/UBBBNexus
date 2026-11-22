#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Animation/AnimSequence.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
const FName RightWrist(TEXT("hand_r"));
const FName LeftWrist(TEXT("hand_l"));
const FName LeftElbow(TEXT("lowerarm_l"));
const FName LeftShoulder(TEXT("upperarm_l"));

bool IsFinger(const FName& Bone)
{
    const FString Name = Bone.ToString();
    return Name.StartsWith(TEXT("thumb_")) || Name.StartsWith(TEXT("index_"))
        || Name.StartsWith(TEXT("middle_")) || Name.StartsWith(TEXT("ring_")) || Name.StartsWith(TEXT("pinky_"));
}

void ComposePose(const FReferenceSkeleton& Skeleton, const TArray<FTransform>& Local, TArray<FTransform>& Space)
{
    for (int32 Bone = 0; Bone < Local.Num(); ++Bone)
    {
        const int32 Parent = Skeleton.GetParentIndex(Bone);
        Space[Bone] = Parent == INDEX_NONE ? Local[Bone] : Local[Bone] * Space[Parent];
    }
}

void ApplyLeftHand(const FReferenceSkeleton& Skeleton, TArray<FTransform>& Local, TArray<FTransform>& Space,
    const FTransform& Target)
{
    const int32 Shoulder = Skeleton.FindBoneIndex(LeftShoulder);
    const int32 Elbow = Skeleton.FindBoneIndex(LeftElbow);
    const int32 Wrist = Skeleton.FindBoneIndex(LeftWrist);
    if (!Space.IsValidIndex(Shoulder) || !Space.IsValidIndex(Elbow) || !Space.IsValidIndex(Wrist) || Target.ContainsNaN())
    {
        return;
    }

    auto Upper = Space[Shoulder];
    auto Forearm = Space[Elbow];
    auto Hand = Space[Wrist];
    const FVector Root = Upper.GetLocation();
    const FVector UpperAxis = Forearm.GetLocation() - Root;
    const FVector ForearmAxis = Hand.GetLocation() - Forearm.GetLocation();
    const double UpperLength = UpperAxis.Size();
    const double ForearmLength = ForearmAxis.Size();
    const FVector Reach = Target.GetLocation() - Root;
    if (UpperLength < 1.0 || ForearmLength < 1.0 || Reach.IsNearlyZero())
    {
        return;
    }

    const FVector Direction = Reach.GetSafeNormal();
    FVector Pole = (UpperAxis - Direction * FVector::DotProduct(UpperAxis, Direction)).GetSafeNormal();
    if (Pole.IsNearlyZero())
    {
        const FVector Down = -FVector::UpVector;
        Pole = (Down - Direction * FVector::DotProduct(Down, Direction)).GetSafeNormal();
        if (Pole.IsNearlyZero())
        {
            Pole = FVector::RightVector;
        }
    }
    const double Distance = FMath::Clamp(Reach.Size(), FMath::Abs(UpperLength - ForearmLength) + 0.5,
        UpperLength + ForearmLength - 0.5);
    const double Run = (UpperLength * UpperLength - ForearmLength * ForearmLength + Distance * Distance) / (2.0 * Distance);
    const FVector Joint = Root + Direction * Run + Pole * FMath::Sqrt(FMath::Max(0.0, UpperLength * UpperLength - Run * Run));
    const FVector End = Root + Direction * Distance;
    Upper.SetRotation((FQuat::FindBetweenVectors(UpperAxis, Joint - Root) * Upper.GetRotation()).GetNormalized());
    Forearm.SetRotation((FQuat::FindBetweenVectors(ForearmAxis, End - Joint) * Forearm.GetRotation()).GetNormalized());
    Forearm.SetTranslation(Joint);
    Hand.SetRotation(Target.GetRotation());
    Hand.SetTranslation(End);

    Local[Shoulder] = Upper.GetRelativeTransform(Space[Skeleton.GetParentIndex(Shoulder)]);
    Local[Elbow] = Forearm.GetRelativeTransform(Upper);
    Local[Wrist] = Hand.GetRelativeTransform(Forearm);
}
}

bool UBBBPlayerItemPortrait::SyncHeldWeapon(const ABBBCharacter& Character)
{
    const auto* Equipment = Cast<ABBBEquipment>(Character.GetHeldItem());
    const auto* Definition = Equipment ? Equipment->GetDefinition() : nullptr;
    const bool bArmed = IsValid(Definition) && IsValid(Definition->EquipmentMesh);
    if (!bArmed)
    {
        Equipment = nullptr;
    }
    if (bHoldInitialized && HeldEquipment.Get() == Equipment && bHoldingWeapon == bArmed)
    {
        return false;
    }

    const bool bChanged = bHoldInitialized;
    bHoldInitialized = true;
    HeldEquipment = Equipment;
    if (bHoldingWeapon != bArmed)
    {
        Pose->PlayAnimation(bArmed ? Stage->GetArmedIdle() : Stage->GetUnarmedIdle(), true);
        bHoldingWeapon = bArmed;
    }
    FingerGrip.Reset();
    if (!Equipment)
    {
        if (Weapon)
        {
            Weapon->SetVisibility(false);
            Weapon->SetSkeletalMesh(nullptr);
            Weapon->EmptyOverrideMaterials();
        }
        return bChanged;
    }

    const FName Socket = Character.GetCharacterConfig().Interaction.RightHandSocketName;
    if (!Weapon)
    {
        if (!ensureMsgf(Pose->DoesSocketExist(Socket), TEXT("整备间骨架缺少右手挂接点 %s"), *Socket.ToString()))
        {
            return bChanged;
        }
        Weapon = NewObject<USkeletalMeshComponent>(Display);
        Display->AddInstanceComponent(Weapon);
        Weapon->SetupAttachment(Pose, Socket);
        Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Weapon->SetVisibleInSceneCaptureOnly(true);
        Weapon->SetForcedLOD(1);
        Weapon->RegisterComponent();
        Weapon->SetComponentTickEnabled(false);
    }
    Weapon->EmptyOverrideMaterials();
    Weapon->SetSkeletalMesh(Definition->EquipmentMesh);
    Weapon->SetRelativeTransform(Definition->SpawnOffset);
    Weapon->SetVisibility(true);
    if (const auto* ActualMesh = Equipment->GetEquipmentSkeletalMesh())
    {
        for (int32 Index = 0; Index < ActualMesh->GetNumMaterials(); ++Index)
        {
            Weapon->SetMaterial(Index, ActualMesh->GetMaterial(Index));
        }
    }

    const UAnimSequence* Grip = Definition->CharacterGripPose;
    const FReferenceSkeleton& CharacterBones = Pose->GetSkeletalMeshAsset()->GetRefSkeleton();
    if (Grip && Grip->GetSkeleton())
    {
        const FReferenceSkeleton& GripBones = Grip->GetSkeleton()->GetReferenceSkeleton();
        for (int32 Index = 0; Index < CharacterBones.GetNum(); ++Index)
        {
            const FName Bone = CharacterBones.GetBoneName(Index);
            if (IsFinger(Bone))
            {
                const int32 GripIndex = GripBones.FindBoneIndex(Bone);
                if (GripIndex != INDEX_NONE)
                {
                    FTransform Finger;
                    Grip->GetBoneTransform(Finger, FSkeletonPoseBoneIndex(GripIndex), FAnimExtractContext(0.0), false);
                    FingerGrip.Add(Index, Finger.GetRotation());
                }
            }
        }
    }
    return bChanged;
}

void UBBBPlayerItemPortrait::ApplyHeldGrip()
{
    const ABBBEquipment* Equipment = HeldEquipment.Get();
    const UBBBEquipmentDefinition* Definition = Equipment ? Equipment->GetDefinition() : nullptr;
    if (!IsValid(Definition))
    {
        return;
    }
    TArray<FTransform> Local = Pose->GetBoneSpaceTransforms();
    TArray<FTransform>& Space = Pose->GetEditableComponentSpaceTransforms();
    const FReferenceSkeleton& Skeleton = Pose->GetSkeletalMeshAsset()->GetRefSkeleton();
    const int32 Wrist = Skeleton.FindBoneIndex(RightWrist);
    if (!Local.IsValidIndex(Wrist) || Local.Num() != Space.Num() || Local.Num() != Skeleton.GetNum())
    {
        return;
    }

    for (const auto& Finger : FingerGrip)
    {
        Local[Finger.Key].SetRotation(Finger.Value);
    }
    const FQuat RightOffset = Definition->RightHandGripRotationOffset.Quaternion();
    Local[Wrist].SetRotation((RightOffset * Local[Wrist].GetRotation()).GetNormalized());
    ComposePose(Skeleton, Local, Space);
    const auto* Animation = Equipment->GetEquipmentAnimationInstance();
    if (Animation && Animation->HasLeftHandTarget())
    {
        const FTransform Grip(Animation->GetLeftHandTargetHandRSpaceRotation(), Animation->GetLeftHandTargetHandRSpace());
        ApplyLeftHand(Skeleton, Local, Space, Grip * Space[Wrist]);
        ComposePose(Skeleton, Local, Space);
    }
    Pose->FinalizeBoneTransform();
    Pose->RefreshFollowerComponents();
    Pose->MarkRenderDynamicDataDirty();
    if (Weapon)
    {
        Weapon->UpdateComponentToWorld();
    }
}
