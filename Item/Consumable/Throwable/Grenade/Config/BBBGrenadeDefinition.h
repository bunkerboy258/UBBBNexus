#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBGrenadeDefinition.generated.h"
class UBBBThrowableDefinition;
class UAnimMontage;
/** 高低抛动作及对应的世界实体资源 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBGrenadeDefinition final : public UBBBConsumableDefinition
{
    GENERATED_BODY()
  public:
    virtual UAnimMontage *GetUseMontage(uint8 Stage) const override
    {
        if (Stage == 1)
        {
            return PrepareMontage;
        }
        if (Stage == 2)
        {
            return HighHoldMontage;
        }
        if (Stage == 3)
        {
            return LowHoldMontage;
        }
        if (Stage == 5)
        {
            return LowThrowMontage;
        }
        return ThrowMontage;
    }
    virtual UBBBThrowableDefinition *GetReleaseDefinition(uint8 Stage) const override
    {
        return Stage == 5 ? LowThrowable.Get() : Throwable.Get();
    }
    virtual float GetReleasePitch(uint8 Stage) const override
    {
        return Stage == 5 ? LowPitchOffset : PitchOffset;
    }
    UBBBGrenadeDefinition()
    {
        MaxStack = 4;
        RuntimeClass = UBBBGrenadeConsumable::StaticClass();
        bThrowable = true;
    }
    /** 高抛世界实体配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "高抛实体"))
    TObjectPtr<UBBBThrowableDefinition> Throwable = nullptr;
    /** 低抛世界实体配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "低抛实体"))
    TObjectPtr<UBBBThrowableDefinition> LowThrowable = nullptr;
    /** 快速高抛及高抛释放动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "高抛动作"))
    TObjectPtr<UAnimMontage> ThrowMontage = nullptr;
    /** 准备动作 随后进入高抛保持 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "准备动作"))
    TObjectPtr<UAnimMontage> PrepareMontage = nullptr;
    /** 高抛循环保持动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "高抛保持"))
    TObjectPtr<UAnimMontage> HighHoldMontage = nullptr;
    /** 低抛循环保持动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "低抛保持"))
    TObjectPtr<UAnimMontage> LowHoldMontage = nullptr;
    /** 低抛释放动作 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "低抛动作"))
    TObjectPtr<UAnimMontage> LowThrowMontage = nullptr;
    /** 高抛相对视线的抬角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "高抛抬角", ClampMin = "-45", ClampMax = "45"))
    float PitchOffset = 12.0f;
    /** 低抛相对视线的抬角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "低抛抬角", ClampMin = "-45", ClampMax = "45"))
    float LowPitchOffset = 3.0f;
};
