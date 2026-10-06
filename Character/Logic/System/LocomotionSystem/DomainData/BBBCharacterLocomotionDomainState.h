#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBCharacterLocomotionDomainState.generated.h"

class FBBBCharacterLocomotionSystem;
class FBBBCharacterLocomotionProcessor;
class FBBBCharacterParseSystem;

/** 角色移动状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterLocomotionDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色当前移动状态 */
    const FBBBCharacterLocomotionState &ReadLocomotionState() const
    {
        return LocomotionState;
    }

    /** @return 当前翻越动作及执行目标 */
    const FBBBCharacterTraversalState &ReadTraversalState() const
    {
        return TraversalState;
    }

private:
    friend class FBBBCharacterTraversalProbeProcessor;
    friend class FBBBCharacterTraversalLifeProcessor;
    friend class FBBBCharacterTraversalWarpProcessor;

    /** 翻越动作的独立执行状态 */
    UPROPERTY(Transient)
    FBBBCharacterTraversalState TraversalState;

    friend class FBBBCharacterLocomotionSystem;
    friend class FBBBCharacterLocomotionProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色当前移动状态 */
    UPROPERTY(Transient)
    FBBBCharacterLocomotionState LocomotionState;
};
