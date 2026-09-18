#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterMontageRequest.generated.h"

class UAnimMontage;
struct FBBBCharacterRuntimeData;

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

    /** @return 蒙太奇是否可进入当前槽位 */
    bool CanApply(const FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    void Apply(FBBBCharacterRuntimeData &Data) const;

    /** @param Data 角色黑板 */
    static void BeginFrame(FBBBCharacterRuntimeData &Data);
};
