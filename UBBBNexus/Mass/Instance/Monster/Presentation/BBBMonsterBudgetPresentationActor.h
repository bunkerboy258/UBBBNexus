#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"
#include "BBBMonsterBudgetPresentationActor.generated.h"

/** 仅供预算动画蓝图使用的表现载体 不改变普通小怪载体的网格类型 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBMonsterBudgetPresentationActor final : public ABBBMonsterPresentationActor
{
    GENERATED_BODY()

public:
    /**
     * 用引擎预算网格替换默认网格 保留无碰撞无玩法 Tick 的载体
     * @param ObjectInitializer	默认子对象初始化器
     * @return 无返回值
     */
    ABBBMonsterBudgetPresentationActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~ Begin AActor Interface
    /** @return 注册引擎预算器 无返回值 */
    virtual void BeginPlay() override;
    //~ End AActor Interface
};
