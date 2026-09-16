#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Definition/Events/BBBEquipmentActionPresentation.h"
#include "BBBReloadDomain.generated.h"

class UAnimMontage;
class UBBBReloadRuntimeData;

/** 换弹动作的装备领域配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBReloadDomain
{
    GENERATED_BODY()

    /** 换弹动作持续时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload", meta = (ClampMin = "0.01"))
    float DurationSeconds = 2.2f;

    /** 换弹人物动作蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Reload")
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** @return 可供动画播放的正值持续时间 */
    float GetDurationSeconds() const;

    /**
     * 构造换弹人物动作表现数据
     * @param OutPresentation	接收人物动作表现数据
     * @return 无
     */
    void BuildPresentation(FBBBEquipmentActionPresentation &OutPresentation) const;
};
