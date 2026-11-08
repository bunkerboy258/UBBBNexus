#pragma once
#include "MassEntityTraitBase.h"
#include "BBBMonsterDirectorTrait.generated.h"

/** 装配副本导演实体 不包含小怪行动片段 */
UCLASS(EditInlineNew, meta = (DisplayName = "BBB 副本小怪导演"))
class ABBB_EVAC_API UBBBMonsterDirectorTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()
protected:
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
