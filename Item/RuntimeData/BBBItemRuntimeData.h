
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemRuntimeData.generated.h"
class UBBBItemDefinition;
class UBBBItemFragmentRuntimeData;

UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemRuntimeData final : public UObject
{
    GENERATED_BODY()
public:

    /**
     * 按静态配置遍历 Fragment 创建并收集运行数据
     * @param Definition	物品静态配置
     */
    void Initialize(const UBBBItemDefinition &Definition);

    /**
     * 按类型查找第一个匹配的运行数据
     * @return 匹配类型的运行数据 未找到时返回 nullptr
     */
    template <typename T>
    T *FindRuntimeData() const
    {

        for (UBBBItemFragmentRuntimeData *RuntimeData : RuntimeDataList)
        {

            if (T *TypedRuntimeData = Cast<T>(RuntimeData))
            {
                return TypedRuntimeData;
            }
        }

        return nullptr;
    }
private:

    UPROPERTY(Instanced)
    TArray<TObjectPtr<UBBBItemFragmentRuntimeData>> RuntimeDataList;
};
