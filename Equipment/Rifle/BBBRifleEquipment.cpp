#include "BBBWork/UBBBNexus/Equipment/Rifle/BBBRifleEquipment.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Definition/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Processors/BBBRifleAnimationProcessor.h"
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
        Slot.bActive = false;
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

    auto &Slot = RuntimeData.Rifle.Input.Equip;
    Slot.Packet = FBBBRifleEquipInput{Sequence};
    Slot.bActive = true;
}

void ABBBRifleEquipment::SubmitPrimaryInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    auto &Slot = RuntimeData.Rifle.Input.Fire;
    Slot.Packet = FBBBRifleFireInput{Sequence};
    Slot.bActive = true;
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

    auto &Slot = RuntimeData.Rifle.Input.Reload;
    Slot.Packet = FBBBRifleReloadInput{Sequence};
    Slot.bActive = true;
}

void ABBBRifleEquipment::SubmitRestoreFact(
    const FBBBEquipmentActionFact &Fact,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || !bInIsMirror)
    {
        return;
    }

    auto &Input = RuntimeData.Rifle.Input;
    if (!ensureMsgf(
        Input.RestoreFactCount < FBBBRifleInputState::MaxRestoreFactCount,
        TEXT("步枪单帧镜像事实超过固定容量 %d"),
        FBBBRifleInputState::MaxRestoreFactCount))
    {
        return;
    }

    Input.RestoreFacts[Input.RestoreFactCount] = FBBBRifleRestoreFactInput{Fact};
    ++Input.RestoreFactCount;
}

void ABBBRifleEquipment::SubmitDetachMagazineInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    auto &Slot = RuntimeData.Rifle.Input.DetachMagazine;
    Slot.Packet = FBBBRifleDetachMagazineInput{Sequence};
    Slot.bActive = true;
}

void ABBBRifleEquipment::SubmitLoadMagazineInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    auto &Slot = RuntimeData.Rifle.Input.LoadMagazine;
    Slot.Packet = FBBBRifleLoadMagazineInput{Sequence};
    Slot.bActive = true;
}

void ABBBRifleEquipment::SubmitInterruptReloadInput(
    const int32 Sequence,
    const bool bInIsMirror)
{
    if (!ValidateInputSource(bInIsMirror) || bInIsMirror)
    {
        return;
    }

    auto &Slot = RuntimeData.Rifle.Input.InterruptReload;
    Slot.Packet = FBBBRifleInterruptReloadInput{Sequence};
    Slot.bActive = true;
}

bool ABBBRifleEquipment::InitializeRuntimeData()
{
    const UBBBRifleDefinition *RifleDefinition = Cast<UBBBRifleDefinition>(GetDefinition());
    if (!ensureMsgf(
        RifleDefinition,
        TEXT("ABBBRifleEquipment 必须使用 UBBBRifleDefinition 初始化")))
    {
        return false;
    }

    RuntimeData = FBBBRifleRuntimeData{};
    RuntimeData.Rifle.Action.AmmoCapacity = FMath::Max(RifleDefinition->AmmoCapacity, 1);
    RuntimeData.Rifle.Action.LoadedAmmo = RuntimeData.Rifle.Action.AmmoCapacity;
    ResetPendingInputs();
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
        ResetPendingInputs();
        return;
    }

    FBBBRifleInputContext Context{
        *this,
        *Character,
        *WeaponMesh,
        *RifleDefinition,
        RuntimeData,
        DeltaSeconds,
        GetWorld()};
    auto &PendingInputs = RuntimeData.Rifle.Input;

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
    ResetPendingInputs();

    const auto &Action = RuntimeData.Rifle.ReadRifleActionState();
    FBBBEquipmentAnimationFacts Facts;
    Facts.bIsReloading = Action.bIsReloading;
    Facts.FireSequence = Action.FireSequence;
    Facts.LastFireTimeSeconds = Action.LastFireTimeSeconds;
    Facts.LoadedAmmo = Action.LoadedAmmo;
    Facts.AmmoCapacity = Action.AmmoCapacity;
    Facts.CurrentWorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    FBBBRifleAnimationProcessor::CalculateLeftHandTarget(Context, Facts);
    AnimationInstance->PublishAnimationFacts(Facts);
}

void ABBBRifleEquipment::ResetPendingInputs()
{
    auto &Input = RuntimeData.Rifle.Input;
    Input.RestoreFactCount = 0;
    Input.Equip.bActive = false;
    Input.DetachMagazine.bActive = false;
    Input.LoadMagazine.bActive = false;
    Input.InterruptReload.bActive = false;
    Input.Reload.bActive = false;
    Input.Fire.bActive = false;
}
