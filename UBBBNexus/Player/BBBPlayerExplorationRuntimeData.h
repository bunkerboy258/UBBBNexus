#pragma once
#include "BBBWork/UBBBNexus/Player/BBBPlayerExplorationDomainState.h"
#include "BBBPlayerExplorationRuntimeData.generated.h"

/** 本地玩家探索运行时黑板 */
USTRUCT()
struct FBBBPlayerExplorationRuntimeData final
{
    GENERATED_BODY()

    /** 唯一探索领域 */
    UPROPERTY()
    FBBBPlayerExplorationDomainState Exploration;
};
