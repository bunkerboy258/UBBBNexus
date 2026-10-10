#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/Processors/BBBShotgunActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ActionSystem/DomainData/Context/BBBShotgunUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/RuntimeData/BBBShotgunRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Config/BBBShotgunDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Config/BBBProjectileDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "MassEntityConfigAsset.h"
#include "NiagaraDataChannelAsset.h"
#include "NiagaraSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

namespace
{
    void Clear(FBBBShotgunActionInputState &Input)
    {
        Input.bEquipRequested = false;
        Input.bActionPermissionReceived = false;
        Input.bBlockFireRequested = false;
        Input.bAllowFireRequested = false;
        Input.bPrimaryRequested = false;
        Input.bReloadRequested = false;
        Input.bLoadAmmoRequested = false;
        Input.bInterruptReloadRequested = false;
        Input.bReloadCycleEnded = false;
    }
}

void FBBBShotgunActionProcessor::Initialize(FBBBShotgunRuntimeData &Data, const UBBBShotgunDefinition &Definition)
{
    auto &State = Data.Action.ActionState;
    State.AmmoCapacity = FMath::Max(1, Definition.AmmoCapacity);
    State.LoadedAmmo = State.AmmoCapacity;
}

void FBBBShotgunActionProcessor::Stop(FBBBShotgunRuntimeData &Data)
{
    Data.Action.ActionState.bFireBlocked = false;
    Data.Action.ActionState.bIsReloading = false;
    Data.Action.ActionState.bReloadCompletedThisFrame = false;
    Data.Action.ActionState.bFireAfterReload = false;
    Data.Action.ActionState.NextFireTimeSeconds = 0.0f;
}

void FBBBShotgunActionProcessor::Update(FBBBShotgunUpdateContext &Context)
{
    auto &Input = Context.RuntimeData.Action.ActionInputState;
    auto &State = Context.RuntimeData.Action.ActionState;
    const bool bPrimaryPressed = Input.bPrimaryRequested && !State.bPrimaryHeld;
    State.bPrimaryHeld = Input.bPrimaryRequested;
    State.bEquippedThisFrame = Input.bEquipRequested;
    if (Context.bCausal)
    {
        State.bReloadCompletedThisFrame = false;
    }

    if (!Context.bCausal)
    {
        if (State.FireSequence != Context.RuntimeData.Animation.ReadShotgunAnimationState().FireSequence
            && Context.RuntimeData.Animation.ReadShotgunAnimationState().bInitialized)
        {
            State.LastFireTimeSeconds = Context.World.GetTimeSeconds();
            SpawnProjectile(Context);
        }

        Clear(Input);
        return;
    }

    // 操作许可先于装匣与开火通知处理 禁止旧通知在攀爬中补结算
    if (Input.bActionPermissionReceived)
    {
        State.bOwnerActionsAllowed = Input.bActionsAllowed;
    }
    if (!State.bOwnerActionsAllowed)
    {
        Stop(Context.RuntimeData);
        State.bEquippedThisFrame = false;
        Clear(Input);
        return;
    }

    // 同帧收到区间结束与就绪通知时允许开火优先 不提前阻塞装备请求帧
    if (Input.bBlockFireRequested)
    {
        State.bFireBlocked = true;
    }
    if (Input.bAllowFireRequested)
    {
        State.bFireBlocked = false;
    }

    if (Input.bLoadAmmoRequested && State.bIsReloading)
    {
        State.LoadedAmmo = FMath::Min(State.AmmoCapacity,
            State.LoadedAmmo + (Context.Definition.bReloadOneRoundAtATime ? 1 : State.AmmoCapacity));
        if (!Context.Definition.bReloadOneRoundAtATime)
        {
            State.bReloadCompletedThisFrame = State.LoadedAmmo >= State.AmmoCapacity;
            State.bIsReloading = !State.bReloadCompletedThisFrame;
        }
        UE_LOG(LogTemp, Log, TEXT("[BBBShotgun] Ammo loaded Equipment=%s Loaded=%d Capacity=%d Completed=%d"),
            *Context.Equipment.GetName(), State.LoadedAmmo, State.AmmoCapacity, State.bReloadCompletedThisFrame);
    }

    // 装入与生命周期结束同帧到达时保留装填成功事实 避免把正常完成误判为打断
    if (Input.bInterruptReloadRequested && !State.bReloadCompletedThisFrame)
    {
        Stop(Context.RuntimeData);
    }

    if (Context.Definition.bReloadOneRoundAtATime && State.bIsReloading && bPrimaryPressed && State.LoadedAmmo > 0)
    {
        State.bFireAfterReload = true;
    }

    if (Input.bReloadCycleEnded && Context.Definition.bReloadOneRoundAtATime && State.bIsReloading)
    {
        if (State.LoadedAmmo >= State.AmmoCapacity || State.bFireAfterReload)
        {
            State.bIsReloading = false;
            State.bReloadCompletedThisFrame = true;
        }
        if (State.bIsReloading)
        {
            ++State.ReloadSequence;
        }
    }

    if (Input.bReloadRequested && !State.bIsReloading && State.LoadedAmmo < State.AmmoCapacity)
    {
        if (ensureMsgf(Context.Definition.CharacterReloadMontage && Context.Definition.EquipmentReloadMontage,
            TEXT("霰弹枪换弹缺少角色或装备蒙太奇")))
        {
            State.bIsReloading = true;
            State.bFireAfterReload = false;
            ++State.ReloadSequence;
        }
    }

    if (Input.bPrimaryRequested)
    {
        UE_LOG(LogTemp, VeryVerbose,
            TEXT("[BBBShotgun] PrimaryRequest Equipment=%s Reloading=%d Ammo=%d Elapsed=%.3f Required=%.3f"),
            *Context.Equipment.GetName(), State.bIsReloading, State.LoadedAmmo,
            Context.World.GetTimeSeconds() - State.LastFireTimeSeconds, Context.Definition.FireInterval);
    }

    const bool bFireRequested = (Input.bPrimaryRequested && (Context.Definition.bAutomaticFire || bPrimaryPressed))
        || State.bFireAfterReload;
    const float CurrentTimeSeconds = Context.World.GetTimeSeconds();
    const float FireInterval = FMath::Max(0.01f, Context.Definition.FireInterval);
    const bool bCanFire = bFireRequested && !State.bFireBlocked && !State.bIsReloading && State.LoadedAmmo > 0;
    if (!bCanFire)
    {
        State.NextFireTimeSeconds = 0.0f;
    }

    if (bCanFire && State.NextFireTimeSeconds <= 0.0f)
    {
        State.NextFireTimeSeconds = FMath::Max(CurrentTimeSeconds, State.LastFireTimeSeconds + FireInterval);
    }

    if (bCanFire && CurrentTimeSeconds >= State.NextFireTimeSeconds)
    {
        if (ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
            TEXT("霰弹枪缺少枪口 Socket")))
        {
            const int32 MaximumShots = Context.Definition.bAutomaticFire && Input.bPrimaryRequested ? 8 : 1;
            for (int32 ShotIndex = 0; ShotIndex < MaximumShots && State.LoadedAmmo > 0 && CurrentTimeSeconds >= State.NextFireTimeSeconds; ++ShotIndex)
            {
                const float ActualFireInterval = CurrentTimeSeconds - State.LastFireTimeSeconds;
                --State.LoadedAmmo;
                State.bFireAfterReload = false;
                ++State.FireSequence;
                State.LastFireTimeSeconds = CurrentTimeSeconds;
                State.NextFireTimeSeconds += FireInterval;

                UE_LOG(LogTemp, VeryVerbose,
                    TEXT("[BBBShotgun] Shot Equipment=%s Definition=%s Sequence=%d ConfiguredInterval=%.3f ActualInterval=%.3f WorldTime=%.3f"),
                    *Context.Equipment.GetName(), *Context.Definition.GetPathName(), State.FireSequence,
                    Context.Definition.FireInterval, ActualFireInterval, CurrentTimeSeconds);

                // 仅本机已成立的开火进入发射扩展 镜像分支在前面返回
                const FTransform MuzzleTransform = Context.WeaponMesh.GetSocketTransform(Context.Definition.MuzzleSocketName);
                SpawnProjectile(Context);

                Context.Equipment.EmitShot(MuzzleTransform);
            }
            if (CurrentTimeSeconds >= State.NextFireTimeSeconds)
            {
                State.NextFireTimeSeconds = CurrentTimeSeconds + FireInterval;
            }
        }
    }

    Clear(Input);
}


void FBBBShotgunActionProcessor::SpawnProjectile(FBBBShotgunUpdateContext& Context)
{
    const UBBBProjectileDefinition* Definition = Context.Definition.ProjectileDefinition;
    UBBBMassSubsystem* Mass = Context.World.GetSubsystem<UBBBMassSubsystem>();
    if (!ensureMsgf(Mass != nullptr && Definition != nullptr && Definition->IsValid(),
        TEXT("霰弹枪缺少有效 Mass 子弹配置 %s"), *Context.Equipment.GetName()))
    {
        return;
    }

    if (!ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
        TEXT("霰弹枪无法取得本机枪口")))
    {
        return;
    }

    FBBBProjectileSpawnLocalControlPacket Packet;
    Packet.MuzzleTransform = Context.WeaponMesh.GetSocketTransform(Context.Definition.MuzzleSocketName);
    Packet.Speed = Definition->InitialSpeedCmPerSecond;
    Packet.GravityScale = Definition->GravityScale;
    Packet.ExplosionRadiusCm = Definition->ExplosionRadiusCm;
    Packet.FuseSeconds = Definition->FuseSeconds;
    Packet.bDetonateOnImpact = Definition->bDetonateOnImpact;
    Packet.bBounceOnImpact = Definition->bBounceOnImpact;
    Packet.BounceRestitution = Definition->BounceRestitution;
    Packet.Mesh = Definition->Mesh.Get();
    Packet.MeshRelativeTransform = Definition->MeshRelativeTransform;
    Packet.Lifetime = Definition->MaximumLifetimeSeconds;
    Packet.Damage = Definition->BaseDamage;
    Packet.DurableDamage = Definition->DurableDamage;
    Packet.Radius = Definition->CollisionRadiusCm;
    Packet.Penetrations = Definition->MaximumPenetrations;
    Packet.PenetrationMultiplier = Definition->PenetrationDamageMultiplier;
    Packet.CollisionChannel = Definition->CollisionChannel;
    Packet.Source = &Context.Equipment;
    Packet.Pawn = &Context.Character;
    Packet.Controller = Context.Character.GetController();
    Packet.Channel = Definition->PresentationChannel.Get();
    Packet.System = Definition->PresentationSystem.Get();
    Packet.ImpactChannel = Definition->ImpactChannel.Get();
    Packet.TracerLengthCm = Definition->TracerLengthCm;
    Packet.TracerWidthCm = Definition->TracerWidthCm;
    Packet.TracerColor = Definition->TracerColor;
    Packet.bCanCauseDamage = Context.bCausal;

    for (int32 Index = 0; Index < Definition->ProjectilesPerShot; ++Index)
    {
        Packet.LocalDirection = FMath::VRandCone(FVector::ForwardVector, FMath::DegreesToRadians(Definition->SpreadHalfAngleDegrees));
        const FMassEntityHandle Entity = Mass->CreateEntity(*Definition->EntityConfig);
        ensureMsgf(Entity.IsSet() && Mass->SubmitInput(Entity, Packet),
            TEXT("霰弹枪 Mass 子弹出生失败 %s"), *Context.Equipment.GetName());
    }
}
