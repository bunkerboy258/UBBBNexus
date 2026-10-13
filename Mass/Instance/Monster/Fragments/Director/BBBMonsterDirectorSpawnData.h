#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDirectorSpawnData.generated.h"

/** 安全创建时机携带的单批出生纯值数据 */
USTRUCT()
struct FBBBMonsterDirectorSpawnData final
{
    GENERATED_BODY()
    /** 对应每只小怪的初始变换 */
    UPROPERTY()
    TArray<FTransform> Transforms;
    /** 当前导演身份 */
    UPROPERTY()
    FGuid DirectorId;
    /** 本次副本运行代号 */
    UPROPERTY()
    uint32 Generation = 0;
    /** 所属阶段 */
    UPROPERTY()
    int32 StageIndex = INDEX_NONE;
};
