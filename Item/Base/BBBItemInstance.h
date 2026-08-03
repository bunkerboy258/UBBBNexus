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

    /**
     * 获取实例唯一标识
     * @return 实例唯一标识
     */
    const FGuid &GetInstanceId() const;

    /**
     * 获取物品静态配置
     * @return 物品静态配置
     */
    UBBBItemDefinition *GetDefinition() const;

    /**
     * 获取物品运行数据
     * @return 物品运行数据
     */
    UBBBItemRuntimeData *GetRuntimeData() const;

    /**
     * 获取堆叠数量
     * @return 当前堆叠数量
     */
    int32 GetStackCount() const;

    /**
     * 校验实例是否有效
     * @return 实例标识与静态配置均有效时返回 true
     */
    bool IsValid() const;

protected:

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemRuntimeData> RuntimeData = nullptr;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 StackCount = 1;
};
