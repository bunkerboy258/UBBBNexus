#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Signature/BBBRifleSignature.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    /** @return 当前解析上下文对应的步枪配置 */
    const UBBBRifleDefinition *GetDefinition(const FBBBEquipmentRuntimeContext &Context)
    {
        return Cast<UBBBRifleDefinition>(Context.Equipment.GetDefinition());
    }

    /**
     * 向角色固定蒙太奇槽提交表现输入
     * @param Context       当前解析上下文
     * @param Montage       待播放蒙太奇
     * @param Sequence      动作序号
     * @param bReload       是否属于换弹流程
     * @return 无
     */
    void SubmitMontage(
        FBBBEquipmentRuntimeContext &Context,
        UAnimMontage *Montage,
        const int32 Sequence,
        const bool bReload)
    {
        if (!Montage)
        {
            return;
        }

        BBBCharacterMontageInput::Submit(Context.Character, *Montage, 1.0f, Sequence, bReload);
    }

    /**
     * 播放不产生玩法结果的开火表现
     * @param Context       当前解析上下文
     * @param Definition    步枪配置
     * @param Sequence      开火序号
     * @return 无
     */
    void PlayFirePresentation(
        FBBBEquipmentRuntimeContext &Context,
        const UBBBRifleDefinition &Definition,
        const int32 Sequence)
    {
        UWorld *World = Context.WeaponMesh.GetWorld();
        if (Definition.FireSound && World
            && Context.WeaponMesh.DoesSocketExist(Definition.MuzzleSocketName))
        {
            const FVector Location = Context.WeaponMesh.GetSocketLocation(Definition.MuzzleSocketName);
            UGameplayStatics::SpawnSoundAtLocation(World, Definition.FireSound, Location);
        }

        SubmitMontage(Context, Definition.FireMontage, Sequence, false);
    }
}

void FBBBRifleSignature::Initialize(FState &State, const UBBBRifleDefinition &Definition)
{
    State.AmmoCapacity = FMath::Max(Definition.AmmoCapacity, 1);
    State.LoadedAmmo = State.AmmoCapacity;
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &,
    const FBBBEquipmentFactInput &,
    FBBBEquipmentRuntimeContext &)
{
    // 本机因果路径产生事实，不允许把事实再次送回同一装备形成回路
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentFactInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition)
    {
        return;
    }

    // 事实包是镜像装备的唯一完整还原入口
    State.LoadedAmmo = FMath::Clamp(Input.Fact.LoadedAmmo, 0, State.AmmoCapacity);

    switch (Input.Fact.PacketId)
    {
        case FBBBEquipFactPacket::PacketId:
            SubmitMontage(Context, Definition->EquipMontage, Input.Fact.Sequence, false);
            return;

        case FBBBFireFactPacket::PacketId:
            State.FireSequence = Input.Fact.Sequence;
            State.LastFireTimeSeconds = Context.WeaponMesh.GetWorld()
                ? Context.WeaponMesh.GetWorld()->GetTimeSeconds()
                : State.LastFireTimeSeconds;
            PlayFirePresentation(Context, *Definition, Input.Fact.Sequence);
            return;

        case FBBBReloadStartedFactPacket::PacketId:
            State.bIsReloading = true;
            State.bMagazineDetached = false;
            State.ReloadSequence = Input.Fact.Sequence;
            SubmitMontage(Context, Definition->ReloadMontage, Input.Fact.Sequence, true);
            return;

        case FBBBMagazineDetachedFactPacket::PacketId:
            State.bMagazineDetached = true;
            return;

        case FBBBMagazineLoadedFactPacket::PacketId:
            State.bIsReloading = false;
            State.bMagazineDetached = false;
            return;

        case FBBBReloadCancelledFactPacket::PacketId:
            State.bIsReloading = false;
            State.bMagazineDetached = false;
            return;

        default:
            ensureMsgf(false, TEXT("步枪收到未知装备事实 PacketId=%u"), Input.Fact.PacketId);
            return;
    }
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &,
    const FBBBEquipmentEquipInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition)
    {
        return;
    }

    SubmitMontage(Context, Definition->EquipMontage, Input.Sequence, false);
}

void FBBBRifleSignature::Restore(
    FState &,
    const FBBBEquipmentEquipInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition)
    {
        return;
    }

    SubmitMontage(Context, Definition->EquipMontage, Input.Sequence, false);
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &,
    const FBBBEquipmentSecondaryInput &,
    FBBBEquipmentRuntimeContext &)
{
    // 当前步枪没有副操作规则，但显式实现签名以保持全装备统一输入表
}

void FBBBRifleSignature::Restore(
    FState &,
    const FBBBEquipmentSecondaryInput &,
    FBBBEquipmentRuntimeContext &)
{
    // 当前步枪没有需要远端还原的副操作事实
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &State,
    const FBBBEquipmentDetachMagazineInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    if (!State.bIsReloading || State.bMagazineDetached)
    {
        return;
    }

    if (Input.Sequence != INDEX_NONE && Input.Sequence != State.ReloadSequence)
    {
        return;
    }

    State.LoadedAmmo = 0;
    State.bMagazineDetached = true;
    Context.Character.SubmitInput(FBBBMagazineDetachedFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentDetachMagazineInput &,
    FBBBEquipmentRuntimeContext &)
{
    State.LoadedAmmo = 0;
    State.bMagazineDetached = true;
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &State,
    const FBBBEquipmentLoadMagazineInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    if (!State.bIsReloading || !State.bMagazineDetached)
    {
        return;
    }

    if (Input.Sequence != INDEX_NONE && Input.Sequence != State.ReloadSequence)
    {
        return;
    }

    State.LoadedAmmo = State.AmmoCapacity;
    State.bIsReloading = false;
    State.bMagazineDetached = false;
    Context.Character.SubmitInput(FBBBMagazineLoadedFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentLoadMagazineInput &,
    FBBBEquipmentRuntimeContext &)
{
    State.LoadedAmmo = State.AmmoCapacity;
    State.bIsReloading = false;
    State.bMagazineDetached = false;
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &State,
    const FBBBEquipmentInterruptReloadInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    if (!State.bIsReloading)
    {
        return;
    }

    if (Input.Sequence != INDEX_NONE && Input.Sequence != State.ReloadSequence)
    {
        return;
    }

    State.bIsReloading = false;
    State.bMagazineDetached = false;
    Context.Character.SubmitInput(FBBBReloadCancelledFactPacket{
        Context.Equipment.GetEquipmentId(),
        State.ReloadSequence,
        State.LoadedAmmo});
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentInterruptReloadInput &,
    FBBBEquipmentRuntimeContext &)
{
    State.bIsReloading = false;
    State.bMagazineDetached = false;
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &State,
    const FBBBEquipmentReloadInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition || !Definition->ReloadMontage)
    {
        return;
    }

    if (State.bIsReloading || State.LoadedAmmo >= State.AmmoCapacity)
    {
        return;
    }

    SubmitMontage(Context, Definition->ReloadMontage, Input.Sequence, true);
    State.bIsReloading = true;
    State.bMagazineDetached = false;
    State.ReloadSequence = Input.Sequence;
    Context.Character.SubmitInput(FBBBReloadStartedFactPacket{
        Context.Equipment.GetEquipmentId(),
        Input.Sequence,
        State.LoadedAmmo});
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentReloadInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition)
    {
        return;
    }

    State.bIsReloading = true;
    State.bMagazineDetached = false;
    State.ReloadSequence = Input.Sequence;
    SubmitMontage(Context, Definition->ReloadMontage, Input.Sequence, true);
}

//------------------------------------------------------------------------------

void FBBBRifleSignature::Resolve(
    FState &State,
    const FBBBEquipmentPrimaryInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    UWorld *World = Context.WeaponMesh.GetWorld();
    if (!Definition || !World)
    {
        return;
    }

    if (State.bIsReloading || State.LoadedAmmo <= 0)
    {
        return;
    }

    if (!Context.WeaponMesh.DoesSocketExist(Definition->MuzzleSocketName))
    {
        ensureMsgf(false, TEXT("步枪网格缺少枪口插槽 %s"), *Definition->MuzzleSocketName.ToString());
        return;
    }

    if (World->GetTimeSeconds() - State.LastFireTimeSeconds < Definition->FireInterval)
    {
        return;
    }

    const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(
        Definition->MuzzleSocketName,
        RTS_World);
    APawn *OwnerPawn = Cast<APawn>(Context.Equipment.GetOwner());
    if (Definition->BulletActorClass && OwnerPawn)
    {
        FActorSpawnParameters Parameters;
        Parameters.Owner = OwnerPawn;
        Parameters.Instigator = OwnerPawn;
        Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABBBBulletActor *Bullet = World->SpawnActor<ABBBBulletActor>(
            Definition->BulletActorClass,
            MuzzleTransform,
            Parameters);
        if (Bullet)
        {
            Bullet->InitializeBullet(
                MuzzleTransform.GetUnitAxis(EAxis::X) * Definition->MuzzleSpeed,
                OwnerPawn,
                &Context.Equipment);
        }
    }

    PlayFirePresentation(Context, *Definition, Input.Sequence);

    // 后坐力是本机控制体验，不属于可镜像还原的游戏事实
    FBBBCameraPacket CameraPacket;
    CameraPacket.Impulse = FVector2D(
        Definition->VerticalRecoilAmount
            + FMath::FRandRange(-Definition->VerticalRecoilRandom, Definition->VerticalRecoilRandom),
        Definition->HorizontalRecoilAmount
            + FMath::FRandRange(-Definition->HorizontalRecoilRandom, Definition->HorizontalRecoilRandom));
    CameraPacket.RecoverySpeed = Definition->RecoilRecoverySpeed;
    Context.Character.SubmitInput(CameraPacket);

    State.LoadedAmmo--;
    State.FireSequence = Input.Sequence;
    State.LastFireTimeSeconds = World->GetTimeSeconds();
    Context.Character.SubmitInput(FBBBFireFactPacket{
        Context.Equipment.GetEquipmentId(),
        Input.Sequence,
        State.LoadedAmmo});
}

void FBBBRifleSignature::Restore(
    FState &State,
    const FBBBEquipmentPrimaryInput &Input,
    FBBBEquipmentRuntimeContext &Context)
{
    const UBBBRifleDefinition *Definition = GetDefinition(Context);
    if (!Definition)
    {
        return;
    }

    State.FireSequence = Input.Sequence;
    State.LastFireTimeSeconds = Context.WeaponMesh.GetWorld()
        ? Context.WeaponMesh.GetWorld()->GetTimeSeconds()
        : State.LastFireTimeSeconds;
    PlayFirePresentation(Context, *Definition, Input.Sequence);
}
