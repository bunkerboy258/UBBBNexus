#pragma once

#include "CoreMinimal.h"

class UAnimMontage;
struct FBBBRifleInputContext;

/** 步枪输入复用的表现操作 */
struct FBBBRiflePresentationProcessor final
{
    /**
     * 将角色蒙太奇贡献交给角色输入入口
     * @param Context	本次解析上下文
     * @param Montage	待贡献的蒙太奇
     * @return 无
     */
    static void SubmitCharacterMontage(
        const FBBBRifleInputContext &Context,
        UAnimMontage *Montage);

    /**
     * 请求当前装备动画实例播放蒙太奇
     * @param Context	本次解析上下文
     * @param Montage	待播放的装备蒙太奇
     * @return 无
     */
    static void PlayEquipmentMontage(
        const FBBBRifleInputContext &Context,
        UAnimMontage *Montage);

    /**
     * 在枪口位置播放开火声音
     * @param Context	本次解析上下文
     * @return 无
     */
    static void PlayFireSound(const FBBBRifleInputContext &Context);
};
