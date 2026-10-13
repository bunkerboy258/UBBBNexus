#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorPhase.h"
#include "BBBMonsterDirectorReplicationState.generated.h"

/** 导演当前结果的机械显示与传输副本 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterDirectorReplicationState final
{
    GENERATED_BODY()

    /** 导演实例身份 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "实例身份"))
    FGuid InstanceId;
    /** 当前运行代号 */
    UPROPERTY()
    uint32 Generation = 0;
    /** 当前结果版本 */
    UPROPERTY()
    uint32 Revision = 0;
    /** 当前阶段索引 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "阶段索引"))
    int32 StageIndex = 0;
    /** 当前阶段标识 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "阶段标识"))
    FName StageId;
    /** 当前运行状态 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "运行状态"))
    EBBBMonsterDirectorPhase Phase = EBBBMonsterDirectorPhase::Inactive;
    /** 累计生成数量 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "累计生成"))
    int32 TotalSpawned = 0;
    /** 本阶段生成数量 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "阶段生成"))
    int32 StageSpawned = 0;
    /** 当前存活数量 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "当前存活"))
    int32 Alive = 0;
    /** 正在申请的数量 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "预留数量"))
    int32 Pending = 0;
    /** 当前等待或失败原因 */
    UPROPERTY(BlueprintReadOnly, Category = "导演", meta = (DisplayName = "当前原因"))
    FString Reason;
};
