#pragma once

#include "MassEntityTypes.h"
#include "BBBSlimeFact.h"
#include "BBBSlimeFragment.generated.h"

class ABBBSlimePresentationActor;

/** 每份粘液唯一的飞行 区域与表现关联状态 */
USTRUCT()
struct ABBB_EVAC_API FBBBSlimeFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前有效结果 */
    UPROPERTY()
    FBBBSlimeFact Fact;
    /** 仅供过滤喷吐时自己的身体 */
    TWeakObjectPtr<AActor> SourceActor;
    /** 当前机械显示载体 */
    TWeakObjectPtr<ABBBSlimePresentationActor> Presentation;
};

/** 对象引用使用引擎复制语义 不持有每实体堆分配 */
template<>
struct TMassFragmentTraits<FBBBSlimeFragment>
{
    enum { AuthorAcceptsItsNotTriviallyCopyable = true };
};
