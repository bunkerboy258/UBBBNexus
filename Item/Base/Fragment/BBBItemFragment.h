
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BBBItemFragment.generated.h"
class UBBBItemFragmentRuntimeData;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBItemFragment : public UObject
{
    GENERATED_BODY()
public:

    /**
     * 创建 Fragment 对应的运行数据 基类默认不创建
     * @param Outer	运行数据生命周期所有者
     * @return 创建的运行数据 默认返回 nullptr
     */
    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const
    {

        return nullptr;
    }
};
