#pragma once

#include "MassMovableVisualizationTrait.h"
#include "BBBProjectileMassTrait.generated.h"

class UWorld;

/** 为实体弹丸配置批处理表现与运行时片段 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "BBB Projectile"))
class ABBB_EVAC_API UBBBProjectileMassTrait final : public UMassMovableVisualizationTrait
{
    GENERATED_BODY()

public:
    /**
     * 创建仅使用实例化静态网格表现的弹丸特征
     * @return 无
     */
    UBBBProjectileMassTrait();

protected:
    /**
     * @param BuildContext	实体模板构建上下文
     * @param World		模板所属世界
     */
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
