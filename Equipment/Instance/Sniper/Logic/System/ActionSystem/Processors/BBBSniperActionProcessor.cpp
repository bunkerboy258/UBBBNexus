#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/Processors/BBBSniperActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ActionSystem/DomainData/Context/BBBSniperUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
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
    void Clear(FBBBSniperActionInputState &Input)
    {
        Input.bEquipRequested = false;
        Input.bActionPermissionReceived = false;
        Input.bBlockFireRequested = false;
        Input.bAllowFireRequested = false;
        Input.bPrimaryRequested = false;
        Input.bReloadRequested = false;
        Input.bLoadMagazineRequested = false;
        Input.bInterruptReloadRequested = false;
    }
}

void FBBBSniperActionProcessor::Initialize(FBBBSniperRuntimeData &Data, const UBBBSniperDefinition &Definition)
{
    auto &State = Data.Action.ActionState;
    State.AmmoCapacity = FMath::Max(1, Definition.AmmoCapacity);
    State.LoadedAmmo = State.AmmoCapacity;
}

void FBBBSniperActionProcessor::Stop(FBBBSniperRuntimeData &Data)
{
    Data.Action.ActionState.bFireBlocked = false;
    Data.Action.ActionState.bIsReloading = false;
    Data.Action.ActionState.bReloadCompletedThisFrame = false;
}

void FBBBSniperActionProcessor::Update(FBBBSniperUpdateContext &Context)
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
        if (State.FireSequence != Context.RuntimeData.Animation.ReadSniperAnimationState().FireSequence
            && Context.RuntimeData.Animation.ReadSniperAnimationState().bInitialized)
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
        State.bIsReloading = false;
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

    if (Input.bLoadMagazineRequested && State.bIsReloading)
    {
        State.LoadedAmmo = State.AmmoCapacity;
        State.bIsReloading = false;
        State.bReloadCompletedThisFrame = true;
        UE_LOG(LogTemp, Log, TEXT("[BBBSniper] Reload completed Equipment=%s Sequence=%d"),
            *Context.Equipment.GetName(), State.ReloadSequence);
    }

    // 装入与生命周期结束同帧到达时保留装填成功事实 避免把正常完成误判为打断
    if (Input.bInterruptReloadRequested && !State.bReloadCompletedThisFrame)
    {
        Stop(Context.RuntimeData);
    }

    if (Input.bReloadRequested && !State.bIsReloading && State.LoadedAmmo < State.AmmoCapacity)
    {
        if (ensureMsgf(Context.Definition.CharacterReloadMontage && Context.Definition.EquipmentReloadMontage,
            TEXT("狙击枪换弹缺少角色或装备蒙太奇")))
        {
            State.bIsReloading = true;
            ++State.ReloadSequence;
        }
    }

    if (Input.bPrimaryRequested)
    {
        UE_LOG(LogTemp, VeryVerbose,
            TEXT("[BBBSniper] PrimaryRequest Equipment=%s Reloading=%d Ammo=%d Elapsed=%.3f Required=%.3f"),
            *Context.Equipment.GetName(), State.bIsReloading, State.LoadedAmmo,
            Context.World.GetTimeSeconds() - State.LastFireTimeSeconds, Context.Definition.FireInterval);
    }

    if (Input.bPrimaryRequested && (Context.Definition.bAutomaticFire || bPrimaryPressed) && !State.bFireBlocked && !State.bIsReloading && State.LoadedAmmo > 0
        && Context.World.GetTimeSeconds() - State.LastFireTimeSeconds >= Context.Definition.FireInterval)
    {
        if (ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
            TEXT("狙击枪缺少枪口 Socket")))
        {
            const float CurrentTimeSeconds = Context.World.GetTimeSeconds();
            const float ActualFireInterval = CurrentTimeSeconds - State.LastFireTimeSeconds;
            --State.LoadedAmmo;
            ++State.FireSequence;
            State.LastFireTimeSeconds = CurrentTimeSeconds;

            UE_LOG(LogTemp, VeryVerbose,
                TEXT("[BBBSniper] Shot Equipment=%s Definition=%s Sequence=%d ConfiguredInterval=%.3f ActualInterval=%.3f WorldTime=%.3f"),
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


void FBBBSniperActionProcessor::SpawnProjectile(FBBBSniperUpdateContext& Context)
{
    const UBBBProjectileDefinition* Definition = Context.Definition.ProjectileDefinition;
    UBBBMassSubsystem* Mass = Context.World.GetSubsystem<UBBBMassSubsystem>();
    if (!ensureMsgf(Mass != nullptr && Definition != nullptr && Definition->IsValid(),
        TEXT("狙击枪缺少有效 Mass 子弹配置 %s"), *Context.Equipment.GetName()))
    {
        return;
    }

    if (!ensureMsgf(Context.WeaponMesh.DoesSocketExist(Context.Definition.MuzzleSocketName),
        TEXT("狙击枪无法取得本机枪口")))
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
            TEXT("狙击枪 Mass 子弹出生失败 %s"), *Context.Equipment.GetName());
    }
}
