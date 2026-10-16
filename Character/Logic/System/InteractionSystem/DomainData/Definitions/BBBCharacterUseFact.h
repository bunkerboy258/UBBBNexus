#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterUseFact.generated.h"
/** 角色已经成立的当前使用结果 不含库存和消耗品网络身份 */
USTRUCT()
struct FBBBCharacterUseFact final
{
    GENERATED_BODY()
    /** 角色范围的结果版本 */
    UPROPERTY()
    uint64 Revision = 0;
    /** 角色范围的使用序号 */
    UPROPERTY()
    int32 Serial = 0;
    /** 静态物品型号 */
    UPROPERTY()
    FName ItemId;
    /** 当前动作阶段 */
    UPROPERTY()
    uint8 Stage = 0;
    /** 当前使用是否占用角色 */
    UPROPERTY()
    bool bActive = false;
    /** 本次是否为角色快速近战 */
    UPROPERTY()
    bool bQuickMelee = false;
    /** 本次释放或恢复已经成立 */
    UPROPERTY()
    bool bCommitted = false;
    /** 已成立的世界释放变换 */
    UPROPERTY()
    FTransform ReleaseTransform = FTransform::Identity;
    /** 释放时角色速度 */
    UPROPERTY()
    FVector InheritedVelocity = FVector::ZeroVector;
};
