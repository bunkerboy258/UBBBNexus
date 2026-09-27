#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/Processors/BBBRifleActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "MassEntityConfigAsset.h"
#include "NiagaraDataChannelAsset.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    void Clear(FBBBRifleActionInputState &Input)
    {
        Input.bEquipRequested = false;
        Input.bPrimaryRequested = false;
        Input.bReloadRequested = false;
        Input.bDetachMagazineRequested = false;
        Input.bLoadMagazineRequested = false;
        Input.bInterruptReloadRequested = false;
        Input.bHasAuthorityFact = false;
        Input.LoadedAmmo = 0;
        Input.FireSequence = 0;
        Input.ReloadSequence = 0;
        Input.bIsReloading = false;
        Input.bMagazineDetached = false;
    }
}

void FBBBRifleActionProcessor::Initialize(FBBBRifleRuntimeData &Data, const UBBBRifleDefinition &Definition)
{
    auto &State = Data.Action.ActionState;
    State.AmmoCapacity = FMath::Max(1, Definition.AmmoCapacity);
    State.LoadedAmmo = State.AmmoCapacity;
}

void FBBBRifleActionProcessor::Stop(FBBBRifleRuntimeData &Data)
{
    Data.Action.ActionState.bIsReloading = false;
    Data.Action.ActionState.bMagazineDetached = false;
}

void FBBBRifleActionProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Action.ActionInputState;
    auto &State = Context.RuntimeData.Action.ActionState;
    State.bEquippedThisFrame = Input.bEquipRequested;

    if (Context.Equipment.IsMirror())
    {
        if (Input.bHasAuthorityFact)
        {
            if (State.FireSequence != Input.FireSequence
                && Context.RuntimeData.Animation.ReadRifleAnimationState().bInitialized)
            {
                State.LastFireTimeSeconds = Context.World.GetTimeSeconds();
                SpawnProjectile(Context);
            }

            State.LoadedAmmo = Input.LoadedAmmo;
            State.FireSequence = Input.FireSequence;
            State.ReloadSequence = Input.ReloadSequence;
            State.bIsReloading = Input.bIsReloading;
            State.bMagazineDetached = Input.bMagazineDetached;
        }

        Clear(Input);
        return;
    }

    // 弹匣通知先于本帧新动作 防止同一帧旧通知完成刚开始的换弹
    if (Input.bDetachMagazineRequested && State.bIsReloading)
    {
        State.bMagazineDetached = true;
    }

    if (Input.bLoadMagazineRequested && State.bIsReloading && State.bMagazineDetached)
    {
        State.LoadedAmmo = State.AmmoCapacity;
        State.bIsReloading = false;
        State.bMagazineDetached = false;
    }

    if (Input.bInterruptReloadRequested)
    {
        Stop(Context.RuntimeData);
    }

    if (Input.bReloadRequested && !State.bIsReloading && State.LoadedAmmo < State.AmmoCapacity)
    {
        if (ensureMsgf(Context.Definition.CharacterReloadMontage && Context.Definition.EquipmentReloadMontage,
            TEXT("步枪换弹缺少角色或装备蒙太奇")))
        {
            State.bIsReloading = true;
            State.bMagazineDetached = false;
            ++State.ReloadSequence;
        }
    }

    if (Input.bPrimaryRequested)
    {
        UE_LOG(LogTemp, VeryVerbose,
            TEXT("[BBBRifle] PrimaryRequest Equipment=%s Reloading=%d Ammo=%d Elapsed=%.3f Required=%.3f"),
            *Context.Equipment.GetName(), State.bIsReloading, State.LoadedAmmo,
            Context.World.GetTimeSeconds() - State.LastFireTimeSeconds, Context.Definition.FireInterval);
    }

    if (Input.bPrimaryRequested && !State.bIsReloading && State.LoadedAmmo > 0
        && Context.World.GetTimeSeconds() - State.LastFireTimeSeconds >= Context.Definition.FireInterval)
    {
        if (ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
            TEXT("步枪缺少枪口 Socket")))
        {
            const float CurrentTimeSeconds = Context.World.GetTimeSeconds();
            const float ActualFireInterval = CurrentTimeSeconds - State.LastFireTimeSeconds;
            --State.LoadedAmmo;
            ++State.FireSequence;
            State.LastFireTimeSeconds = CurrentTimeSeconds;

            UE_LOG(LogTemp, VeryVerbose,
                TEXT("[BBBRifle] Shot Equipment=%s Definition=%s Sequence=%d ConfiguredInterval=%.3f ActualInterval=%.3f WorldTime=%.3f"),
                *Context.Equipment.GetName(), *Context.Definition.GetPathName(), State.FireSequence,
                Context.Definition.FireInterval, ActualFireInterval, CurrentTimeSeconds);

            // 仅本机已成立的开火进入发射扩展 镜像分支在前面返回
            const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(Context.Definition.MuzzleSocketName);
            SpawnProjectile(Context);

            Context.Equipment.EmitShot(MuzzleTransform);
        }
    }

    Clear(Input);
}


void FBBBRifleActionProcessor::SpawnProjectile(FBBBRifleUpdateContext& Context)
{
    const UBBBProjectileDefinition* Definition = Context.Definition.ProjectileDefinition;
    UBBBMassSubsystem* Mass = Context.World.GetSubsystem<UBBBMassSubsystem>();
    if (!ensureMsgf(Mass != nullptr && Definition != nullptr && Definition->IsValid(),
        TEXT("步枪缺少有效 Mass 子弹配置 %s"), *Context.Equipment.GetName()))
    {
        return;
    }

    if (!ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
        TEXT("步枪无法取得本机枪口")))
    {
        return;
    }

    FBBBProjectileSpawnLocalControlPacket Packet;
    Packet.MuzzleTransform = Context.WeaponMesh.GetSocketTransform(Context.Definition.MuzzleSocketName);
    Packet.Speed = Definition->InitialSpeedCmPerSecond;
    Packet.Lifetime = Definition->MaximumLifetimeSeconds;
    Packet.Damage = Definition->BaseDamage;
    Packet.Radius = Definition->CollisionRadiusCm;
    Packet.Penetrations = Definition->MaximumPenetrations;
    Packet.PenetrationMultiplier = Definition->PenetrationDamageMultiplier;
    Packet.CollisionChannel = Definition->CollisionChannel;
    Packet.Source = &Context.Equipment;
    Packet.Pawn = &Context.Character;
    Packet.Controller = Context.Character.GetController();
    Packet.Channel = Definition->PresentationChannel.Get();
    Packet.bCanCauseDamage = !Context.Equipment.IsMirror();

    const FMassEntityHandle Entity = Mass->CreateEntity(*Definition->EntityConfig);
    ensureMsgf(Entity.IsSet() && Mass->SubmitInput(Entity, MoveTemp(Packet)),
        TEXT("步枪 Mass 子弹出生失败 %s"), *Context.Equipment.GetName());
}
