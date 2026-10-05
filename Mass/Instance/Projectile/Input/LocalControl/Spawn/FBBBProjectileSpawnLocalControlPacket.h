#pragma once

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileCollisionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"

struct FBBBProjectileSpawnInputFragment;

/** 本机枪口出生数据 镜像仅关闭造伤权限 */
struct FBBBProjectileSpawnLocalControlPacket final
{
    using FInputFragment = FBBBProjectileSpawnInputFragment;

    FTransform MuzzleTransform = FTransform::Identity;
    float Speed = 50000.0f;
    float Lifetime = 5.0f;
    float Damage = 20.0f;
    float Radius = 2.0f;
    int32 Penetrations = 0;
    float PenetrationMultiplier = 0.65f;
    ECollisionChannel CollisionChannel = ECC_Pawn;
    TWeakObjectPtr<AActor> Source;
    TWeakObjectPtr<APawn> Pawn;
    TWeakObjectPtr<AController> Controller;
    TWeakObjectPtr<UNiagaraDataChannelAsset> Channel;
    TWeakObjectPtr<UNiagaraSystem> System;
    /** 当帧表面反馈通道 */
    TWeakObjectPtr<UNiagaraDataChannelAsset> ImpactChannel;
    float TracerLengthCm = 1000.0f;
    float TracerWidthCm = 2.5f;
    FLinearColor TracerColor = FLinearColor(20.0f, 8.0f, 1.0f, 1.0f);
    bool bCanCauseDamage = false;

    /** @return 出生数据是否有效 */
    bool IsValid() const
    {
        return !MuzzleTransform.ContainsNaN()
            && FMath::IsFinite(Speed) && Speed > 0.0f
            && FMath::IsFinite(Lifetime) && Lifetime > 0.0f
            && FMath::IsFinite(Damage) && Damage >= 0.0f
            && FMath::IsFinite(Radius) && Radius >= 0.0f
            && Penetrations >= 0 && Penetrations <= 32
            && FMath::IsFinite(PenetrationMultiplier)
            && PenetrationMultiplier >= 0.0f && PenetrationMultiplier <= 1.0f
            && Channel.IsValid() && System.IsValid()
            && ImpactChannel.IsValid()
            && FMath::IsFinite(TracerLengthCm) && TracerLengthCm > 0.0f
            && FMath::IsFinite(TracerWidthCm) && TracerWidthCm > 0.0f
            && FMath::IsFinite(TracerColor.R)
            && FMath::IsFinite(TracerColor.G)
            && FMath::IsFinite(TracerColor.B)
            && FMath::IsFinite(TracerColor.A);
    }

    /** @return 是否尚未完成出生 */
    bool CanApply(const FBBBProjectileMotionFragment& Motion) const
    {
        return !Motion.bInitialized;
    }

    /**
     * @param Transform	位置状态
     * @param Velocity	速度状态
     * @param Motion	运动状态
     * @param Collision	碰撞状态
     * @param Life	寿命状态
     * @param Presentation	表现事实
     * @return 无
     */
    void Apply(FTransformFragment& Transform, FMassVelocityFragment& Velocity,
        FBBBProjectileMotionFragment& Motion, FBBBProjectileCollisionFragment& Collision,
        FBBBProjectileLifetimeFragment& Life, FBBBProjectilePresentationFragment& Presentation) const
    {
        Transform.GetMutableTransform() = MuzzleTransform;
        Transform.GetMutableTransform().SetScale3D(FVector::OneVector);
        Velocity.Value = MuzzleTransform.GetUnitAxis(EAxis::X) * Speed;
        Motion.SpawnLocation = MuzzleTransform.GetLocation();
        Motion.PreviousLocation = MuzzleTransform.GetLocation();
        Motion.bInitialized = true;
        Collision.Damage = Damage;
        Collision.CollisionRadiusCm = Radius;
        Collision.RemainingPenetrations = Penetrations;
        Collision.PenetrationDamageMultiplier = PenetrationMultiplier;
        Collision.CollisionChannel = CollisionChannel;
        Collision.DamageCauser = Source;
        Collision.InstigatorPawn = Pawn;
        Collision.EventInstigator = Controller;
        Collision.bCanCauseDamage = bCanCauseDamage;
        Life.RemainingSeconds = Lifetime;
        Presentation.Channel = Channel;
        Presentation.System = System;
        Presentation.ImpactChannel = ImpactChannel;
        Presentation.LengthCm = TracerLengthCm;
        Presentation.WidthCm = TracerWidthCm;
        Presentation.Color = TracerColor;
    }
};
