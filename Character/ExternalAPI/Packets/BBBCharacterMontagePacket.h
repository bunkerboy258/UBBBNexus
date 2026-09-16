#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontagePacket.generated.h"

class UAnimMontage;

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
};
