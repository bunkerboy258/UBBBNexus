#pragma once

#include "MassEntityTraitBase.h"
#include "BBBProjectileTrait.generated.h"

/** 只装配子弹逻辑数据 不要求表现网格 */
UCLASS(EditInlineNew, meta = (DisplayName = "BBB Projectile"))
class ABBB_EVAC_API UBBBProjectileTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()

protected:
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
