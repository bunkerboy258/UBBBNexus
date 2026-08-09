#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Aim/BBBAimDomin.h"
#include "BBBAimFragment.generated.h"

class UAnimSequence;

/**
 * 装备瞄准动画配置
 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBAimFragment final : public FBBBAimDomin
{
    GENERATED_BODY()

public:
    /** @return 瞄准期间使用的上半身动画 */
    virtual UAnimSequence *GetUpperBodyAnimation() const override;

    /** 瞄准期间使用的上半身动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Aim")
    TObjectPtr<UAnimSequence> UpperBodyAnimation = nullptr;
};
