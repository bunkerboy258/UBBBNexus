#pragma once
#include "CoreMinimal.h"
#include "Animation/PoseSnapshot.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/DomainData/Definitions/BBBCharacterRagdollSnapshot.h"
#include "BBBCharacterPhysicalPresentationState.generated.h"

/** 局部受击与全身物理表现的独立应用记录 */
USTRUCT()
struct FBBBCharacterPhysicalPresentationState final
{
    GENERATED_BODY()
    /** 最新已经表现的命中 */
    uint64 HitSerial = 0;
    /** 已应用的全身冲击版本 */
    uint64 ImpactSerial = 0;
    /** 本次进入物理时需要叠加的速度 */
    FVector PendingImpactVelocity = FVector::ZeroVector;
    /** 被击倒后最早允许安全起身的本机世界时间 零表示无自动起身 */
    double ImpactReleaseAt = 0;
    /** 全身布娃娃是否已经接管网格 */
    bool bRagdoll = false;
    /** 输入接受的全身物理请求 与已经执行的接管标记分离 */
    bool bRagdollRequested = false;
    /** 等待安全落地的起身请求 */
    bool bGetUpRequested = false;
    /** 已校验的当前起身空间与速度条件 */
    bool bCanGetUp = false;
    /** 当前候选起身胶囊变换 */
    FTransform GetUpTransform;
    /** 自然结束通知已通过轮次校验 */
    bool bGetUpComplete = false;
    /** 本机物理退出前的网格变换 供动画重定位快照 */
    FTransform RagdollMeshTransform;
    /** 交还身体控制之前的实际骨骼姿态 */
    UPROPERTY(Transient)
    FPoseSnapshot ExitPose;
    /** 当前身体曾在倒地生命阶段等待救援 */
    bool bWasDowned = false;
    /** 控制端采集或镜像输入还原的当前物理结果 */
    UPROPERTY()
    FBBBCharacterRagdollSnapshot RagdollSnapshot;
    /** 镜像已应用到物理身体的结果版本 */
    uint64 AppliedRagdollRevision = 0;
    /** 本机物理完成后采集的骨盆位置 */
    FVector PelvisLocation = FVector::ZeroVector;
};
