#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBCharacterTraversalDomainState.generated.h"

/** 攀爬系统的唯一业务状态持有者 */
USTRUCT()
struct FBBBCharacterTraversalDomainState final
{
    GENERATED_BODY()

public:
    /** @return 当前翻越动作及执行目标 */
    const FBBBCharacterTraversalState &ReadTraversalState() const
    {
        return TraversalState;
    }

private:
    friend class FBBBCharacterTraversalSystem;
    friend class FBBBCharacterTraversalProbeProcessor;
    friend class FBBBCharacterTraversalLifeProcessor;
    friend class FBBBCharacterTraversalWarpProcessor;
    friend class FBBBCharacterParseSystem;

    /** 翻越动作的独立执行状态 */
    UPROPERTY(Transient)
    FBBBCharacterTraversalState TraversalState;
};
