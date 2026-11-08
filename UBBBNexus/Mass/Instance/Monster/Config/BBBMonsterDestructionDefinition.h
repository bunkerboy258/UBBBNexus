#pragma once
#include "UObject/Object.h"
#include "BBBMonsterDestructionRegionDefinition.h"
#include "BBBMonsterDestructionDefinition.generated.h"
class USkeletalMesh;

/** 八区躯干毁伤的共享制作资源 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, HideDropdown, meta=(DisplayName="毁伤系统"))
class ABBB_EVAC_API UBBBMonsterDestructionDefinition final : public UObject
{
    GENERATED_BODY()

public:
    /** 顺序为正面左胸 右胸 左腹 右腹 背面左上背 右上背 左腰背 右腰背 */
    UPROPERTY(EditAnywhere, EditFixedSize, Category="毁伤", meta=(DisplayName="八区创口", TitleProperty="Name"))
    TArray<FBBBMonsterDestructionRegionDefinition> Regions;

    /** 一份蒙皮网格承载共享内部组织与八处肉壁 */
    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="内部组织与肉壁"))
    TObjectPtr<USkeletalMesh> Interior;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="每次命中加深量", ClampMin="0.01", ClampMax="1"))
    float HitAccumulation = 0.35f;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="开口阈值", ClampMin="0.01", ClampMax="1"))
    float OpeningThreshold = 0.65f;
};
