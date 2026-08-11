#pragma once

#include "MassEntityTraitBase.h"

#include "MonsterMassTrait.generated.h"

/** 定义小怪实体模板的核心组成 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Monster Core"))
class ABBB_EVAC_API UMonsterMassTrait final : public UMassEntityTraitBase
{
    GENERATED_BODY()

protected:
    /**
     * 将小怪核心数据加入实体模板
     * @param BuildContext	实体模板构建上下文
     * @param World		模板所属世界
     */
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
