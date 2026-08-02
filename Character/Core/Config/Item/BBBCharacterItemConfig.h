
#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterItemConfig.generated.h"
class UBBBItemDefinition;

USTRUCT(BlueprintType)
//描述角色出生时创建的一类库存物品
struct FBBBDefaultInventoryItem
{
    GENERATED_BODY()

    //指定用于创建实例的物品静态定义
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    //指定加入主库存的初始堆叠数量
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 StackCount = 1;

    //指定物品实例同时绑定的可选快捷栏槽位
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "-1"))
    int32 HotbarSlot = INDEX_NONE;
};

USTRUCT(BlueprintType)
//定义角色库存容量默认物品与自动装备策略
struct FBBBCharacterItemConfig
{
    GENERATED_BODY()

    //限制主库存能够保存的条目数量
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 MainInventoryCapacity = 20;

    //限制快捷栏能够映射的物品实例数量
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 HotbarCapacity = 5;

    //列出角色本地初始化时创建的物品
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FBBBDefaultInventoryItem> DefaultItems;

    //决定初始化后是否选择首个有效快捷栏物品
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bAutoEquipFirstHotbarItem = true;
};
