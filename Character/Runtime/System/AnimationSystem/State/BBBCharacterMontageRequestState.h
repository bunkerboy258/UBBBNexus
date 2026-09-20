#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontageRequestState.generated.h"

class UAnimMontage;
struct FBBBAnimationState;
struct FBBBCharacterParseState;

/**
 *
 * 动画槽位中跨帧驻留的蒙太奇播放请求
 */
USTRUCT()
struct FBBBCharacterMontageRequestState
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UAnimMontage> Montage = nullptr;

    UPROPERTY()
    float PlayRate = 1.0f;

    UPROPERTY()
    int32 Sequence = INDEX_NONE;

    UPROPERTY()
    bool bReload = false;

    /**
     *
     * 检查请求是否可进入当前槽位
     * @param Animation    动画运行时状态
     * @param Operation    解析状态机状态
     * @param Slot         明确目标槽位
     * @return 是否可写入目标槽位
     */
    bool CanApply(
        const FBBBAnimationState &Animation,
        const FBBBCharacterParseState &Operation,
        FName Slot) const;

    /**
     *
     * 写入槽位期望并分配播放修订号
     * @param Animation    动画运行时状态
     * @param Slot         明确目标槽位
     */
    void Apply(FBBBAnimationState &Animation, FName Slot) const;

    /**
     *
     * 帧初撤销已失效的槽位请求
     * @param Animation    动画运行时状态
     * @param Operation    解析状态机状态
     */
    static void BeginFrame(FBBBAnimationState &Animation, const FBBBCharacterParseState &Operation);
};
