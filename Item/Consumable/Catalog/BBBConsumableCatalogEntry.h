#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBConsumableCatalogEntry.generated.h"
class ABBBConsumable;

/** 单一物品类别的静态注册 */
USTRUCT(BlueprintType)
struct FBBBConsumableCatalogEntry final
{
    GENERATED_BODY()
    /** 注册的静态物品定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "物品定义"))
    TObjectPtr<UBBBConsumableDefinition> Definition = nullptr;

    /** 该物品的抽象运行时类 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "实例类"))
    TSubclassOf<ABBBConsumable> ActorClass;
};
