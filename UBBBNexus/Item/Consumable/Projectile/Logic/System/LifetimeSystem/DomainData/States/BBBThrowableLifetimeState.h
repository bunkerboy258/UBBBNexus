#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableBounceFact.h"
class UBBBThrowableDefinition;
/** 初始化和引信共享生命周期 */
struct FBBBThrowableLifetimeState final
{
    /** 释放时缓存的投掷者身份 离线后仍保留伤害归属 */
    int32 PlayerId = INDEX_NONE;
    /** 等待目标自身处理的战斗贡献 */
    TArray<FBBBHitContribution> Hits;
    /** 当前是否已释放 */
    bool bLaunched = false;
    /** 释放时世界时间 */
    double LaunchTime = 0.0;
    /** 等待初始化的静态定义 */
    UBBBThrowableDefinition* PendingDefinition = nullptr;
    /** 等待初始化的释放变换 */
    FTransform PendingTransform = FTransform::Identity;
    /** 释放时持有者速度 */
    FVector PendingVelocity = FVector::ZeroVector;

    /** 本帧真实碰撞中最大的速度变化 */
    float PendingBounceStrength = 0.0f;
    /** 本帧最大碰撞的接触位置 */
    FVector PendingBouncePosition = FVector::ZeroVector;
    /** 最近一次可听碰撞结果 */
    FBBBThrowableBounceFact Bounce;
    /** 本端已经播放的碰撞序号 */
    uint32 PresentedBounceSerial = 0;
    /** 已观察到本机物理运动 防止初始复制前误停尾迹 */
    bool bTrailHasMoved = false;
    /** 最近一次弹跳声音时间 */
    double LastBounceSoundTime = -1.0;
};
