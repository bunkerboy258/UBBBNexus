#include "BBBWork/UBBBNexus/Equipment/System/Animation/BBBEquipmentAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Reload/Definition/BBBReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBEquipmentAnimationSystem::Update(
    USkeletalMeshComponent &CharacterMesh,
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterEquipmentEvents &EquipmentEvents,
    const float WorldTimeSeconds)
{
    UBBBEquipmentInstance *Instance = EquipmentState.GetActiveMainHandInstance();
    if (!Instance)
    {
        LastInstance.Reset();
        LastAnimInstance.Reset();
        Facts = FBBBEquipmentAnimationFacts();
        return;
    }

    ABBBEquipmentPresentationActor *PresentationActor = Instance->PresentationActor;
    USkeletalMeshComponent *WeaponMesh = PresentationActor
        ? Cast<USkeletalMeshComponent>(PresentationActor->GetEquipmentAttachmentComponent())
        : nullptr;
    UBBBEquipmentAnimInstance *AnimInstance = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    if (!ensureMsgf(
        Instance->RuntimeData && WeaponMesh && AnimInstance,
        TEXT("[UBBBE]Equipment animation snapshot has no runtime data, weapon mesh or animation instance")))
    {
        return;
    }

    if (LastInstance.Get() != Instance || LastAnimInstance.Get() != AnimInstance)
    {
        Facts = FBBBEquipmentAnimationFacts();
        LastInstance = Instance;
        LastAnimInstance = AnimInstance;
    }

    Facts.CurrentWorldTimeSeconds = WorldTimeSeconds;
    Facts.bHasMainHandEquipment = true;

    const UBBBEquipRuntimeData *Equip = Instance->RuntimeData->GetEquip();
    if (!ensureMsgf(Equip, TEXT("[UBBBE]Equipment animation snapshot has no equip runtime data")))
    {
        return;
    }

    Facts.bHasValidAimSource = Equip->bHasValidAimSource;
    Facts.AimSourceLocalTransform = Equip->bHasValidAimSource
        ? Equip->AimSourceRightHandBoneSpace
        : FTransform::Identity;

    Facts.bHasLeftHandTarget = CharacterMesh.GetBoneIndex(TEXT("hand_r")) != INDEX_NONE
        && WeaponMesh->DoesSocketExist(Equip->LeftHandSocketName);
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    if (Facts.bHasLeftHandTarget)
    {
        const FVector SocketPosition = WeaponMesh->GetSocketTransform(
            Equip->LeftHandSocketName,
            RTS_Component).GetLocation();
        const FVector TargetWorld = WeaponMesh->GetComponentTransform().TransformPosition(
            SocketPosition + Equip->LeftHandSocketOffset);
        Facts.LeftHandTargetHandRSpace = CharacterMesh.GetBoneTransform(
            TEXT("hand_r"),
            RTS_World).InverseTransformPosition(TargetWorld) + Equip->LeftHandIKOffset;
    }

    if (!ensureMsgf(Facts.bHasLeftHandTarget, TEXT("[UBBBE]Weapon left hand socket or character hand_r bone is missing")))
    {
        Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    }

    const UBBBFireRuntimeData *Fire = Instance->RuntimeData->GetFire();
    Facts.LastFireTimeSeconds = Fire
        ? Fire->GetLastFireTime()
        : -1000.0f;

    const UBBBReloadRuntimeData *Reload = Instance->RuntimeData->GetReload();
    Facts.bIsReloading = Reload && Reload->IsReloading();
    if (Reload)
    {
        Facts.ReloadStartTimeSeconds = Reload->GetStartTimeSeconds();
        Facts.ReloadDurationSeconds = Reload->GetDurationSeconds();
        Facts.ReloadSequence = Reload->GetSequence();
    }

    for (const FBBBEquipmentActionEvent &Event : EquipmentEvents.GetActionEvents())
    {
        if (Instance->GetDefinition() && Event.EquipmentId != Instance->GetDefinition()->EquipmentId)
        {
            continue;
        }

        Facts.EquipmentActionType = Event.ActionType;
        Facts.EquipmentActionSequence = Event.Sequence;
        Facts.EquipmentActionDuration = Event.DurationSeconds;
        Facts.EquipmentActionMontage = Event.Presentation.Montage;
        Facts.EquipmentActionPlayRate = Event.Presentation.PlayRate;

        if (Event.ActionType == EBBBCharacterActionType::Fire)
        {
            Facts.FireSequence++;
        }

    }

    AnimInstance->PublishAnimationFacts(Facts);
}
