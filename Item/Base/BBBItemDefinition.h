
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

    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Item")
    TArray<TObjectPtr<UBBBItemFragment>> Fragments;

    /**
     * 按类型查找静态配置中的第一个匹配 Fragment
     * @return 匹配类型的 Fragment 未找到时返回 nullptr
     */
    template <typename T>
    const T *FindFragment() const
    {
        for (const UBBBItemFragment *Fragment : Fragments)
        {
            if (const T *TypedFragment = Cast<T>(Fragment))
            {
                return TypedFragment;
            }
        }
        return nullptr;
    }
};
