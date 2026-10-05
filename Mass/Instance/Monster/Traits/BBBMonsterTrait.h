#pragma once

#include "MassEntityTraitBase.h"
#include "BBBMonsterTrait.generated.h"

class UBBBMonsterDefinition;

/** 装配小怪实体数据 */
UCLASS(EditInlineNew, meta = (DisplayName = "BBB 小怪"))
class ABBB_EVAC_API UBBBMonsterTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()

public:
    /** 当前实体类型的静态配置 */
    UPROPERTY(EditAnywhere, Category = "BBB|小怪", meta = (DisplayName = "定义资产"))
    TObjectPtr<UBBBMonsterDefinition> Definition;

protected:
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
