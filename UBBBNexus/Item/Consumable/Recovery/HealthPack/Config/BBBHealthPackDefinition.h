#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/BBBHealthPackConsumable.h"
#include "BBBHealthPackDefinition.generated.h"
class UAnimMontage;
/** 一次生效的急救药包静态定义 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBHealthPackDefinition final : public UBBBConsumableDefinition
{
    GENERATED_BODY()
  public:
    virtual UAnimMontage *GetUseMontage(const UBBBAnimInstance *Animation) const override
    {
        return UseMontage;
    }
    virtual float GetRecoveryFraction() const override
    {
        return RecoveryFraction;
    }
    UBBBHealthPackDefinition()
    {
        RuntimeClass = UBBBHealthPackConsumable::StaticClass();
        bThrowable = false;
        MaxStack = 4;
    }
    /** 完整使用动作 生效和结束由通知控制 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "使用动作"))
    TObjectPtr<UAnimMontage> UseMontage = nullptr;
    /** 生效时恢复的最大生命比例 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
              meta = (DisplayName = "生命恢复比例", ClampMin = "0.001", ClampMax = "1"))
    float RecoveryFraction = 0.30f;
};
