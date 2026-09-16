#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/Definition/BBBEquipRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Fire/Definition/BBBFireResults.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Reload/Definition/BBBReloadRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/BBBEquipmentPresentationActor.h"
#include "BBBWork/UBBBNexus/Equipment/Presentation/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/RunTime/BBBEquipmentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

UBBBEquipmentInstance *UBBBEquipmentInstance::Create(
    UObject &Outer,
    UBBBEquipmentDefinition &InDefinition)
{
    if (!ensureMsgf(InDefinition.EquipDomin.IsValid(), TEXT("[UBBBE]Equipment definition has no equip domain")))
    {
        return nullptr;
    }

    UBBBEquipmentInstance *Instance = NewObject<UBBBEquipmentInstance>(&Outer);
    if (!ensureMsgf(Instance, TEXT("[UBBBE]Equipment instance creation failed")))
    {
        return nullptr;
    }

    Instance->InstanceId = FGuid::NewGuid();
    Instance->Definition = &InDefinition;
    Instance->RuntimeData = NewObject<UBBBEquipmentRuntimeData>(Instance);
    if (!ensureMsgf(Instance->RuntimeData, TEXT("[UBBBE]Equipment runtime data creation failed")))
    {
        return nullptr;
    }

    Instance->RuntimeData->Initialize(InDefinition);
    if (!ensureMsgf(
        Instance->RuntimeData->GetEquip()
            && (!InDefinition.FireDomin.IsValid() || Instance->RuntimeData->GetFire())
            && (!InDefinition.ReloadDomain.IsValid() || Instance->RuntimeData->GetReload()),
        TEXT("[UBBBE]Equipment domain runtime data is incomplete")))
    {
        return nullptr;
    }

    return Instance;
}

//------------------------------------------------------------------------------

const FGuid &UBBBEquipmentInstance::GetInstanceId() const
{
    return InstanceId;
}

//------------------------------------------------------------------------------

UBBBEquipmentDefinition *UBBBEquipmentInstance::GetDefinition() const
{
    return Definition;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::BindHolder(
    USkeletalMeshComponent &InCharacterMesh,
    const FName InAttachmentSocketName)
{
    if (!ensureMsgf(!InAttachmentSocketName.IsNone(), TEXT("[UBBBE]Equipment holder attachment socket is missing")))
    {
        return false;
    }

    HolderMesh = &InCharacterMesh;
    AttachmentSocketName = InAttachmentSocketName;
    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::Activate()
{
    if (PresentationActor)
    {
        return true;
    }

    USkeletalMeshComponent *CharacterMesh = HolderMesh.Get();
    if (!ensureMsgf(
        CharacterMesh && Definition && Definition->EquipDomin.IsValid() && RuntimeData && RuntimeData->GetEquip(),
        TEXT("[UBBBE]Equipment activation dependencies are invalid")))
    {
        return false;
    }

    PresentationActor = Definition->EquipDomin.Get().Equip(
        *RuntimeData->GetEquip(),
        *CharacterMesh,
        AttachmentSocketName);
    if (!PresentationActor)
    {
        return false;
    }

    USkeletalMeshComponent *WeaponMesh = Cast<USkeletalMeshComponent>(PresentationActor->GetEquipmentAttachmentComponent());
    UBBBEquipmentAnimInstance *WeaponAnim = WeaponMesh
        ? Cast<UBBBEquipmentAnimInstance>(WeaponMesh->GetAnimInstance())
        : nullptr;
    UBBBAnimInstance *CharacterAnim = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    if (!ensureMsgf(WeaponAnim && CharacterAnim, TEXT("[UBBBE]Equipped weapon or character animation instance has an invalid class")))
    {
        Deactivate();
        return false;
    }

    BoundCharacterAnimInstance = CharacterAnim;
    BoundWeaponAnimInstance = WeaponAnim;
    CharacterAnim->BindWeaponAnimInstance(WeaponAnim);
    WeaponMesh->PrimaryComponentTick.AddPrerequisite(
        CharacterMesh,
        CharacterMesh->PrimaryComponentTick);
    return true;
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::Deactivate()
{
    UBBBAnimInstance *CharacterAnim = BoundCharacterAnimInstance.Get();
    UBBBEquipmentAnimInstance *WeaponAnim = BoundWeaponAnimInstance.Get();
    USkeletalMeshComponent *CharacterMesh = HolderMesh.Get();
    USkeletalMeshComponent *WeaponMesh = WeaponAnim
        ? WeaponAnim->GetSkelMeshComponent()
        : nullptr;
    if (CharacterMesh && WeaponMesh)
    {
        WeaponMesh->PrimaryComponentTick.RemovePrerequisite(
            CharacterMesh,
            CharacterMesh->PrimaryComponentTick);
    }

    if (CharacterAnim && CharacterAnim->TryGetWeaponAnimInstance() == WeaponAnim)
    {
        CharacterAnim->BindWeaponAnimInstance(nullptr);
    }

    BoundCharacterAnimInstance.Reset();
    BoundWeaponAnimInstance.Reset();

    if (PresentationActor)
    {
        PresentationActor->Destroy();
        PresentationActor = nullptr;
    }
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::Shutdown()
{
    Deactivate();
    HolderMesh.Reset();
    AttachmentSocketName = NAME_None;
    PendingActions.Reset();
    PendingRecoils.Reset();

    if (RuntimeData && RuntimeData->GetReload())
    {
        RuntimeData->GetReload()->bIsReloading = false;
    }
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::SubmitFire(const int32 Sequence)
{
    if (!ensureMsgf(
        PresentationActor && Definition && Definition->FireDomin.IsValid() && RuntimeData && RuntimeData->GetFire(),
        TEXT("[UBBBE]Equipment fire domain is unavailable")))
    {
        return false;
    }

    FBBBEquipmentFireResult FireResult;
    if (!Definition->FireDomin.Get().Fire(
        *PresentationActor,
        *RuntimeData->GetFire(),
        FireResult))
    {
        return false;
    }

    FBBBEquipmentActionEvent Event;
    Event.ActionType = EBBBCharacterActionType::Fire;
    Event.EquipmentId = Definition->EquipmentId;
    Event.Sequence = Sequence;
    Definition->FireDomin.Get().BuildFireActionPresentation(Event.Presentation);
    PendingActions.Add(MoveTemp(Event));

    FBBBEquipmentRecoilEvent Recoil;
    Recoil.Impulse = FireResult.RecoilImpulse;
    Recoil.RecoverySpeed = FireResult.RecoilRecoverySpeed;
    PendingRecoils.Add(MoveTemp(Recoil));
    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::SubmitReload(
    const float WorldTimeSeconds,
    const int32 Sequence)
{
    UBBBReloadRuntimeData *Reload = RuntimeData
        ? RuntimeData->GetReload()
        : nullptr;
    if (!ensureMsgf(
        PresentationActor && Definition && Definition->ReloadDomain.IsValid() && Reload,
        TEXT("[UBBBE]Equipment reload domain is unavailable; migrate the equipment asset ReloadDomain")))
    {
        return false;
    }

    if (Reload->bIsReloading)
    {
        return false;
    }

    const FBBBReloadDomain &Domain = Definition->ReloadDomain.Get();
    Reload->bIsReloading = true;
    Reload->StartTimeSeconds = WorldTimeSeconds;
    Reload->DurationSeconds = Domain.GetDurationSeconds();
    Reload->Sequence = Sequence;

    FBBBEquipmentActionEvent Event;
    Event.ActionType = EBBBCharacterActionType::Reload;
    Event.EquipmentId = Definition->EquipmentId;
    Event.Sequence = Sequence;
    Event.DurationSeconds = Reload->DurationSeconds;
    Domain.BuildPresentation(Event.Presentation);
    PendingActions.Add(MoveTemp(Event));
    return true;
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::ApplyRestoredAction(
    const FBBBEquipmentActionEvent &Event,
    const float WorldTimeSeconds)
{
    if (!Definition || !PresentationActor || Event.EquipmentId != Definition->EquipmentId)
    {
        return false;
    }

    FBBBEquipmentActionEvent PresentedEvent = Event;
    if (Event.ActionType == EBBBCharacterActionType::Fire)
    {
        if (!ensureMsgf(Definition->FireDomin.IsValid() && RuntimeData && RuntimeData->GetFire(), TEXT("[UBBBE]Restored fire domain is unavailable")))
        {
            return false;
        }

        Definition->FireDomin.Get().Present(*PresentationActor, *RuntimeData->GetFire());
        Definition->FireDomin.Get().BuildFireActionPresentation(PresentedEvent.Presentation);
        PendingActions.Add(MoveTemp(PresentedEvent));
        return true;
    }

    if (Event.ActionType == EBBBCharacterActionType::Reload)
    {
        UBBBReloadRuntimeData *Reload = RuntimeData
            ? RuntimeData->GetReload()
            : nullptr;
        if (!ensureMsgf(Definition->ReloadDomain.IsValid() && Reload, TEXT("[UBBBE]Restored reload domain is unavailable")))
        {
            return false;
        }

        Reload->bIsReloading = true;
        Reload->StartTimeSeconds = WorldTimeSeconds;
        Reload->DurationSeconds = Event.DurationSeconds > 0.0f
            ? Event.DurationSeconds
            : Definition->ReloadDomain.Get().GetDurationSeconds();
        Reload->Sequence = Event.Sequence;
        PresentedEvent.DurationSeconds = Reload->DurationSeconds;
        Definition->ReloadDomain.Get().BuildPresentation(PresentedEvent.Presentation);
        PendingActions.Add(MoveTemp(PresentedEvent));
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::AdvanceAction(const float WorldTimeSeconds)
{
    UBBBReloadRuntimeData *Reload = RuntimeData
        ? RuntimeData->GetReload()
        : nullptr;
    if (!Reload || !Reload->bIsReloading)
    {
        return;
    }

    if (WorldTimeSeconds - Reload->StartTimeSeconds >= Reload->DurationSeconds)
    {
        Reload->bIsReloading = false;
    }
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::ConsumeEvents(FBBBCharacterEquipmentEvents &OutEvents)
{
    for (FBBBEquipmentActionEvent &Event : PendingActions)
    {
        OutEvents.AddAction(MoveTemp(Event));
    }

    for (FBBBEquipmentRecoilEvent &Event : PendingRecoils)
    {
        OutEvents.AddRecoil(MoveTemp(Event));
    }

    PendingActions.Reset();
    PendingRecoils.Reset();
}

//------------------------------------------------------------------------------

bool UBBBEquipmentInstance::IsReloading() const
{
    const UBBBReloadRuntimeData *Reload = RuntimeData
        ? RuntimeData->GetReload()
        : nullptr;
    return Reload && Reload->IsReloading();
}

//------------------------------------------------------------------------------

float UBBBEquipmentInstance::GetEquipDuration() const
{
    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid(), TEXT("[UBBBE]Equipment equip duration is unavailable")))
    {
        return 0.0f;
    }

    return Definition->EquipDomin.Get().GetEquipDuration();
}

//------------------------------------------------------------------------------

float UBBBEquipmentInstance::GetReloadDuration() const
{
    if (!ensureMsgf(Definition && Definition->ReloadDomain.IsValid(), TEXT("[UBBBE]Equipment reload domain is unavailable")))
    {
        return 0.0f;
    }

    return Definition->ReloadDomain.Get().GetDurationSeconds();
}

//------------------------------------------------------------------------------

void UBBBEquipmentInstance::BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const
{
    OutPresentation.Montage = nullptr;
    OutPresentation.PlayRate = 1.0f;

    if (!ensureMsgf(Definition && Definition->EquipDomin.IsValid(), TEXT("[UBBBE]Equipment equip presentation is unavailable")))
    {
        return;
    }

    Definition->EquipDomin.Get().BuildEquipActionPresentation(OutPresentation);
}
