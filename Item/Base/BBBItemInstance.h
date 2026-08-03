#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemInstance.generated.h"

class UBBBItemDefinition;
class UBBBItemRuntimeData;

/** 物品实例根对象 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBItemInstance : public UObject
{
    GENERATED_BODY()

public:

    /** @return 物品实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 物品静态配置 */
    UBBBItemDefinition *GetDefinition() const;

    /** @return 物品持久运行数据 */
    UBBBItemRuntimeData *GetRuntimeData() const;

    /** @return 当前堆叠数量 */
    int32 GetStackCount() const;

    /** @return 实例是否具备合法身份与配置 */
    bool IsValid() const;

protected:

    /**
     * 写入实例基础身份
     * @param InDefinition\t物品静态配置
     * @param InStackCount\t堆叠数量
     */
    void Configure(UBBBItemDefinition &InDefinition, int32 InStackCount);

    /** 物品实例唯一标识 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    /** 物品静态配置引用 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    /** 物品持久运行数据引用 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemRuntimeData> RuntimeData = nullptr;

    /** 当前堆叠数量 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 StackCount = 1;
};
