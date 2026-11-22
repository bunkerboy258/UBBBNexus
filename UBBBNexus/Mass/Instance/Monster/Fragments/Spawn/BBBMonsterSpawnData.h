#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnSource.h"
#include "BBBMonsterSpawnData.generated.h"

/** 安全创建时机携带的单批出生纯值数据 */
USTRUCT()
struct FBBBMonsterSpawnData final
{
    GENERATED_BODY()
    /** 对应每只小怪的初始变换 */
    UPROPERTY()
    TArray<FTransform> Transforms;
    /** 与每个变换对应的出生种子 */
    UPROPERTY()
    TArray<uint32> Seeds;
    /** 当前导演或生成器身份 */
    UPROPERTY()
    FGuid DirectorId;
    /** 本次副本运行代号 */
    UPROPERTY()
    uint32 Generation = 0;
    /** 出生调度来源 */
    UPROPERTY()
    EBBBMonsterSpawnSource Source = EBBBMonsterSpawnSource::Spawner;
    /** 预制波次标识 非预制波次为 None */
    UPROPERTY()
    FName EventId;
    /** 本次出生可选的初始目标玩家编号 */
    UPROPERTY()
    int32 InitialTargetPlayerId = INDEX_NONE;
    /** 出生时若无显式目标是否自动锁定最近存活玩家 */
    UPROPERTY()
    bool bAlertOnSpawn = false;
};
