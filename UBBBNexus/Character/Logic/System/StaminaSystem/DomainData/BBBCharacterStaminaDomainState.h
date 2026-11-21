#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/StaminaSystem/DomainData/States/BBBCharacterStaminaState.h"
#include "BBBCharacterStaminaDomainState.generated.h"

class FBBBCharacterStaminaProcessor;
class FBBBCharacterParseSystem;

/** 角色体力领域全部状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterStaminaDomainState final
{
    GENERATED_BODY()

    /** @return 当前体力结果 */
    const FBBBCharacterStaminaState &ReadStaminaState() const
    {
        return StaminaState;
    }

  private:
    friend class FBBBCharacterStaminaProcessor;
    friend class FBBBCharacterParseSystem;

    /** 当前体力结果 */
    UPROPERTY(Transient)
    FBBBCharacterStaminaState StaminaState;
};
