#pragma once
#include "BBBWork/UBBBNexus/Player/BBBPlayerExplorationState.h"
#include "BBBPlayerExplorationDomainState.generated.h"

/** 本地玩家探索领域的唯一状态持有者 */
USTRUCT()
struct FBBBPlayerExplorationDomainState final
{
    GENERATED_BODY()
public:
    /** @return 当前探索只读状态 */
    const FBBBPlayerExplorationState &ReadPlayerExplorationState() const
    {
        return State;
    }

private:
    friend class FBBBPlayerExplorationProcessor;

    UPROPERTY()
    FBBBPlayerExplorationState State;
};
