#pragma once

#include "GameFramework/GameUserSettings.h"
#include "BBBGameUserSettings.generated.h"

/** 在引擎画质应用流程中统一选择项目抗锯齿方法与动画预算 */
UCLASS(Config=GameUserSettings)
class ABBB_EVAC_API UBBBGameUserSettings final : public UGameUserSettings
{
    GENERATED_BODY()

public:
    //~ Begin UGameUserSettings Interface
    /** @return 应用标准画质分组 抗锯齿方法与动画预算 不保存额外状态 */
    virtual void ApplyNonResolutionSettings() override;
    //~ End UGameUserSettings Interface
};
