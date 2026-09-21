#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/DomainData/States/BBBCharacterLocomotionState.h"
#include "BBBCharacterLocomotionDomainState.generated.h"

class FBBBCharacterLocomotionController;
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

private:
    friend class FBBBCharacterLocomotionController;
    friend class FBBBCharacterLocomotionProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色当前移动状态 */
    UPROPERTY(Transient)
    FBBBCharacterLocomotionState LocomotionState;
};
