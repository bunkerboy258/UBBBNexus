#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontageRequest.generated.h"

class UAnimMontage;

/**
 *
 * 动画槽位中跨帧驻留的蒙太奇播放请求
 */
USTRUCT()
struct FBBBCharacterMontageRequest final
{
    GENERATED_BODY()

    /** 期望播放的蒙太奇 */
    UPROPERTY()
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** 蒙太奇播放速率 */
    UPROPERTY()
    float PlayRate = 1.0f;

    /** 请求所属操作序号 */
    UPROPERTY()
    int32 Sequence = INDEX_NONE;

    /** 请求是否属于换弹流程 */
    UPROPERTY()
    bool bReload = false;
};
