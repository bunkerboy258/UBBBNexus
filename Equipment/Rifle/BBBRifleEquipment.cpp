#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    /**
     * 解析并清除一个固定输入槽
     * @param Slot       待解析输入槽
     * @param Context    步枪输入上下文
     * @return 无
     */
    template<typename TPacket>
    void ParseInputSlot(
        TBBBEquipmentInputSlot<TPacket> &Slot,
        FBBBRifleInputContext &Context)
    {
        if (!Slot.bActive)
        {
            return;
        }

        const TPacket Packet = Slot.Packet;
        Slot.Reset();
        if (!ensureMsgf(Packet.IsValid(), TEXT("步枪收到无效输入包")))
        {
            return;
        }

        if (!Packet.CanApply(Context))
        {
            return;
        }

        Packet.Apply(Context);
    }
}

ABBBRifleEquipment::ABBBRifleEquipment()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    PrimaryActorTick.EndTickGroup = TG_PostUpdateWork;

    USkeletalMeshComponent *WeaponMesh = GetEquipmentSkeletalMesh();
    if (WeaponMesh)
    {
        PrimaryActorTick.AddPrerequisite(WeaponMesh, WeaponMesh->PrimaryComponentTick);
    }
}

void ABBBRifleEquipment::SubmitEquipInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.Equip.Submit(FBBBRifleEquipInput{Sequence});
}

void ABBBRifleEquipment::SubmitPrimaryInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.Fire.Submit(FBBBRifleFireInput{Sequence});
}

void ABBBRifleEquipment::SubmitSecondaryInput(
    const bool,
    const int32,
    const bool bInIsMirror)
{
    ValidateInputSource(bInIsMirror);
}

void ABBBRifleEquipment::SubmitReloadInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.Reload.Submit(FBBBRifleReloadInput{Sequence});
}

void ABBBRifleEquipment::SubmitRestoreFact(
    const FBBBEquipmentActionFact &Fact,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || !bInIsMirror)
    {
        return;
    }

    ensureMsgf(
        PendingInputs.SubmitRestoreFact(FBBBRifleRestoreFactInput{Fact}),
        TEXT("步枪单帧镜像事实超过固定容量 %d"),
        FBBBRifleInputFrame::MaxRestoreFactCount);
}

void ABBBRifleEquipment::SubmitDetachMagazineInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.DetachMagazine.Submit(FBBBRifleDetachMagazineInput{Sequence});
}

void ABBBRifleEquipment::SubmitLoadMagazineInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.LoadMagazine.Submit(FBBBRifleLoadMagazineInput{Sequence});
}

void ABBBRifleEquipment::SubmitInterruptReloadInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    PendingInputs.InterruptReload.Submit(FBBBRifleInterruptReloadInput{Sequence});
}

bool ABBBRifleEquipment::InitializeRuntimeData(UBBBEquipmentDefinition &InDefinition)
{
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(&InDefinition);
    if (!ensureMsgf(
        RifleDefinition,
        TEXT("ABBBRifleEquipment 必须使用 UBBBRifleDefinition 初始化")))
    {
        return false;
    }

    RuntimeData = FBBBRifleRuntimeData{};
    RuntimeData.AmmoCapacity = FMath::Max(RifleDefinition->AmmoCapacity, 1);
    RuntimeData.LoadedAmmo = RuntimeData.AmmoCapacity;
    PendingInputs.Reset();
    return true;
}

void ABBBRifleEquipment::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ABBBCharacter *Character = Cast<ABBBCharacter>(GetOwner());
    USkeletalMeshComponent *WeaponMesh = GetEquipmentSkeletalMesh();
    UBBBEquipmentAnimInstance *AnimationInstance = GetEquipmentAnimationInstance();
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    if (!ensureMsgf(
        Character && WeaponMesh && AnimationInstance && RifleDefinition,
        TEXT("步枪更新缺少角色、网格、动画实例或配置")))
    {
        PendingInputs.Reset();
        return;
    }

    FBBBRifleInputContext Context{
        *this,
        *Character,
        *WeaponMesh,
        *RifleDefinition,
        RuntimeData,
        DeltaSeconds};

    for (int32 Index = 0; Index < PendingInputs.RestoreFactCount; ++Index)
    {
        FBBBRifleRestoreFactInput &Input = PendingInputs.RestoreFacts[Index];
        if (!ensureMsgf(Input.IsValid(), TEXT("步枪收到无效镜像事实输入")))
        {
            continue;
        }

        if (Input.CanApply(Context))
        {
            Input.Apply(Context);
        }
    }

    ParseInputSlot(PendingInputs.Equip, Context);
    ParseInputSlot(PendingInputs.DetachMagazine, Context);
    ParseInputSlot(PendingInputs.LoadMagazine, Context);
    ParseInputSlot(PendingInputs.InterruptReload, Context);
    ParseInputSlot(PendingInputs.Reload, Context);
    ParseInputSlot(PendingInputs.Fire, Context);
    PendingInputs.Reset();

    FBBBEquipmentAnimationFacts Facts;
    Facts.bIsReloading = RuntimeData.bIsReloading;
    Facts.FireSequence = RuntimeData.FireSequence;
    Facts.LastFireTimeSeconds = RuntimeData.LastFireTimeSeconds;
    Facts.LoadedAmmo = RuntimeData.LoadedAmmo;
    Facts.AmmoCapacity = RuntimeData.AmmoCapacity;
    Facts.CurrentWorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    AnimationInstance->PublishAnimationFacts(Facts);
}
