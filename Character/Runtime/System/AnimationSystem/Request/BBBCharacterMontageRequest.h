#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontageRequest.generated.h"

class UAnimMontage;
struct FBBBAnimationState;
struct FBBBCharacterParseState;

/** 装备向角色贡献的本地蒙太奇播放数据包 */
USTRUCT()
struct FBBBCharacterMontagePacket
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
     * 检查蒙太奇是否可进入当前槽位
     * @param Animation	动画运行时数据
     * @param Operation	解析状态机
     * @param Slot      明确目标槽位
     * @return 蒙太奇是否可进入目标槽位
     */
    bool CanApply(
        const FBBBAnimationState &Animation,
        const FBBBCharacterParseState &Operation,
        FName Slot) const;

    /**
     * 写入槽位期望并分配修订号
     * @param Animation 动画运行时数据
     * @param Slot      明确目标槽位
     */
    void Apply(FBBBAnimationState &Animation, FName Slot) const;

    /**
     * 帧初初始化槽位并撤销失效期望
     * @param Animation	动画运行时数据
     * @param Operation	解析状态机
     */
    static void BeginFrame(FBBBAnimationState &Animation, const FBBBCharacterParseState &Operation);
};
