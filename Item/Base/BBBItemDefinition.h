
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBItemDefinition.generated.h"
class UBBBItemFragment;
class UTexture2D;

UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FName ItemId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item", meta = (ClampMin = "1"))
    int32 MaxStack = 1;

    //收集本定义全部 Fragment 命名插槽 供运行数据生成 基类无插槽 由子类追加
    virtual void CollectFragments(TArray<const UBBBItemFragment *> &OutFragments) const
    {
    }
};
