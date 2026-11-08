#pragma once
#include "MassEntityTraitBase.h"
#include "BBBMonsterSpawnerTrait.generated.h"

/** 仅装配统一出生请求的状态与输入 */
UCLASS(meta = (DisplayName = "小怪生成入口"))
class ABBB_EVAC_API UBBBMonsterSpawnerTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()
protected:
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
