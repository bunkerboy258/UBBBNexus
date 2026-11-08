#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceStyle.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/Definitions/BBBCharacterAppearanceSnapshot.h"
#include "BBBCharacterAppearanceInputState.generated.h"

/** 角色外观的待消费输入状态 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceInputState
{
    GENERATED_BODY()

    /** 待染色的实例 */
    TArray<FGuid> PendingColorInstances;
    /** 基础染色部件 */
    TArray<FName> PendingColorSlots;
    /** 染色区域颜色 */
    TArray<TArray<FLinearColor>> PendingColors;
    /** 待调整迷彩的实例 */
    TArray<FGuid> PendingCamouflageInstances;
    /** 基础迷彩部件 */
    TArray<FName> PendingCamouflageSlots;
    /** 迷彩开关 */
    TArray<bool> PendingCamouflage;
    /** 本机基础部件选择 */
    TArray<FName> PendingBaseSlots;
    /** 与基础部件一一对应的资源标识 */
    TArray<FName> PendingBaseResources;
    /** 整体污渍请求 */
    TArray<float> PendingDirt;
    /** 整体磨损请求 */
    TArray<float> PendingWeathering;
    /** 已接收的完整既成结果 按提交顺序消费 */
    TArray<FBBBCharacterAppearanceSnapshot> PendingSelections;

};
