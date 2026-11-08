#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/States/BBBCharacterPickupState.h"
#include "BBBCharacterPickupDomainState.generated.h"
/** 角色拾取领域的唯一状态持有者 */
USTRUCT()
struct FBBBCharacterPickupDomainState final
{
    GENERATED_BODY()
public:
    /** @return 只读的拾取过程与待投送事实 */
    const FBBBCharacterPickupState &ReadCharacterPickupState() const
    {
        return PickupState;
    }
private:
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterPickupProcessor;
    /** 拾取业务状态 */
    UPROPERTY()
    FBBBCharacterPickupState PickupState;
};
