#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/BBBGrenadeConsumable.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
#include "BBBGrenadeDefinition.generated.h"
class UBBBThrowableDefinition;
/** 一次快速投掷及世界实体资源 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBGrenadeDefinition final : public UBBBConsumableDefinition
{
    GENERATED_BODY()
  public:
    virtual UAnimMontage *GetUseMontage(const UBBBAnimInstance *Animation, FName EquipmentId) const override
    {
        if (const TObjectPtr<UAnimMontage> *Montage = EquipmentThrowMontages.Find(EquipmentId))
        {
            return Montage->Get();
        }
        return Animation ? Animation->SelectGrenadeThrowMontage() : nullptr;
    }
    virtual UBBBThrowableDefinition *GetReleaseDefinition() const override
    {
        return Throwable;
    }
    virtual float GetReleasePitch() const override
    {
        return PitchOffset;
    }
    UBBBGrenadeDefinition()
    {
        MaxStack = 4;
        RuntimeClass = UBBBGrenadeConsumable::StaticClass();
        bThrowable = true;
    }
    /** 释放后的世界实体配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "投掷实体"))
    TObjectPtr<UBBBThrowableDefinition> Throwable = nullptr;
    /** 指定装备时使用的投掷蒙太奇 以装备物品标识匹配 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "装备专用投掷动作"))
    TMap<FName, TObjectPtr<UAnimMontage>> EquipmentThrowMontages;
    /** 相对视线的投掷抬角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "投掷抬角", ClampMin = "-45", ClampMax = "45"))
    float PitchOffset = 12.0f;
};
