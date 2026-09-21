
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"
#include "BBBAimDomainState.generated.h"

class FBBBCharacterAimController;
class FBBBCharacterAimStateProcessor;
class FBBBCharacterAimTargetProcessor;
class FBBBCharacterParseSystem;

USTRUCT(BlueprintType)
/** 角色瞄准状态的唯一持有者 */
struct FBBBAimDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色当前瞄准状态 */
    const FBBBAimState &ReadAimState() const
    {
        return AimState;
    }

private:
    friend class FBBBCharacterAimController;
    friend class FBBBCharacterAimStateProcessor;
    friend class FBBBCharacterAimTargetProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色当前瞄准状态 */
    UPROPERTY(Transient)
    FBBBAimState AimState;
};
