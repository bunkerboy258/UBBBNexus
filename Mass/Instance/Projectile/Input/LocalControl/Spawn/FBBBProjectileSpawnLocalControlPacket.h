#pragma once

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Engine/StaticMesh.h"
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
    /** 枪口局部空间的初始方向 */
    FVector LocalDirection = FVector::ForwardVector;
    /** 世界重力倍率 */
    float GravityScale = 0.0f;
    /** 范围伤害半径 */
    float ExplosionRadiusCm = 0.0f;
    /** 延时引信 零表示没有引信 */
    float FuseSeconds = 0.0f;
    /** 接触目标时引爆 */
    bool bDetonateOnImpact = true;
    /** 未引爆时接触反弹 */
    bool bBounceOnImpact = false;
    /** 反弹速度倍率 */
    float BounceRestitution = 0.4f;
    /** 批量弹体网格 */
    TWeakObjectPtr<UStaticMesh> Mesh;
    /** 弹体网格局部变换 */
    FTransform MeshRelativeTransform = FTransform::Identity;
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
            && !LocalDirection.ContainsNaN() && !LocalDirection.IsNearlyZero()
            && FMath::IsFinite(GravityScale) && GravityScale >= 0.0f
            && FMath::IsFinite(ExplosionRadiusCm) && ExplosionRadiusCm >= 0.0f
            && FMath::IsFinite(FuseSeconds) && FuseSeconds >= 0.0f && FuseSeconds <= Lifetime
            && (FuseSeconds == 0.0f || ExplosionRadiusCm > 0.0f)
            && (ExplosionRadiusCm == 0.0f || (Penetrations == 0 && (bDetonateOnImpact || FuseSeconds > 0.0f)))
            && FMath::IsFinite(BounceRestitution) && BounceRestitution >= 0.0f && BounceRestitution <= 1.0f
            && !MeshRelativeTransform.ContainsNaN()
            && FMath::IsFinite(Speed) && Speed > 0.0f
            && FMath::IsFinite(Lifetime) && Lifetime > 0.0f
            && FMath::IsFinite(Damage) && Damage >= 0.0f
            && FMath::IsFinite(Radius) && Radius >= 0.0f
            && Penetrations >= 0 && Penetrations <= 32
            && FMath::IsFinite(PenetrationMultiplier)
            && PenetrationMultiplier >= 0.0f && PenetrationMultiplier <= 1.0f
            && (Mesh.IsValid() || (Channel.IsValid() && System.IsValid()))
            && (Channel.IsValid() == System.IsValid())
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
        const FVector Direction = MuzzleTransform.TransformVectorNoScale(LocalDirection).GetSafeNormal();
        Velocity.Value = Direction * Speed;
        Transform.GetMutableTransform().SetRotation(Direction.ToOrientationQuat());
        Motion.GravityScale = GravityScale;
        Motion.bResting = false;
        Motion.SpawnLocation = MuzzleTransform.GetLocation();
        Motion.PreviousLocation = MuzzleTransform.GetLocation();
        Motion.bInitialized = true;
        Collision.Damage = Damage;
        Collision.ExplosionRadiusCm = ExplosionRadiusCm;
        Collision.bDetonateOnImpact = bDetonateOnImpact;
        Collision.bBounceOnImpact = bBounceOnImpact;
        Collision.BounceRestitution = BounceRestitution;
        Collision.CollisionRadiusCm = Radius;
        Collision.RemainingPenetrations = Penetrations;
        Collision.PenetrationDamageMultiplier = PenetrationMultiplier;
        Collision.CollisionChannel = CollisionChannel;
        Collision.DamageCauser = Source;
        Collision.InstigatorPawn = Pawn;
        Collision.EventInstigator = Controller;
        Collision.bCanCauseDamage = bCanCauseDamage;
        Life.RemainingSeconds = Lifetime;
        Life.FuseRemainingSeconds = FuseSeconds;
        Presentation.Mesh = Mesh;
        Presentation.MeshRelativeTransform = MeshRelativeTransform;
        Presentation.Channel = Channel;
        Presentation.System = System;
        Presentation.ImpactChannel = ImpactChannel;
        Presentation.LengthCm = TracerLengthCm;
        Presentation.WidthCm = TracerWidthCm;
        Presentation.Color = TracerColor;
    }
};
