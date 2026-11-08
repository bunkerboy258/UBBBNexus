#pragma once

#include "MassEntityTraitBase.h"
#include "BBBSlimeTrait.generated.h"

/** 装配粘液当前结果与出生输入 */
UCLASS(EditInlineNew, meta = (DisplayName = "BBB 粘液"))
class ABBB_EVAC_API UBBBSlimeTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()
protected:
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
