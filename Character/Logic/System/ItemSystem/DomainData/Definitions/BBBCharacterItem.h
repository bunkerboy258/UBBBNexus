#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBCharacterItem.generated.h"
class AActor;

/** 角色持有的单件物品及其稳定身份 */
USTRUCT(BlueprintType)
struct FBBBCharacterItem final
{
    GENERATED_BODY()
    /** 单件身份 移动格子时保持不变 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "实例标识"))
    FGuid InstanceId;
    /** 共享的型号定义 空定义表示空格子 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "物品定义"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;
    /** 主动装备持有的运行时实体 穿戴品与杂物为空 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "装备实例"))
    TObjectPtr<AActor> HeldInstance = nullptr;

    /** 本堆叠的唯一数量 零数量仅等待使用结束后回收 */
    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "数量"))
    int32 Quantity = 0;
};
