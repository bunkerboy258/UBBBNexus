#pragma once
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "BBBConsumableDefinition.generated.h"
class UStaticMesh;
class UAnimInstance;

/** 消耗品共有的静态持有资源 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBConsumableDefinition : public UBBBItemDefinition
{
    GENERATED_BODY()
public:
    UBBBConsumableDefinition()
    {
        ItemType = EBBBItemType::Consumable;
    }

    /** 手持时显示的静态网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "手持网格"))
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    /** 角色手部挂接骨骼或插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "持有挂点"))
    FName HandSocketName = TEXT("hand_r");

    /** 相对角色手部挂点的变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "持有偏移"))
    FTransform SpawnOffset = FTransform::Identity;

    /** 角色使用此物品时链接的动画层 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|消耗品", meta = (DisplayName = "角色动画层"))
    TSubclassOf<UAnimInstance> CharacterAnimationLayerClass;
};
