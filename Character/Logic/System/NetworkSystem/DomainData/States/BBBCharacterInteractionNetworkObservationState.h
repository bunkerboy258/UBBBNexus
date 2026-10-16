#pragma once

#include "CoreMinimal.h"

/** 角色实际持有关系的发送基准 */
struct FBBBCharacterInteractionNetworkObservationState final
{
    /** 已发送的实际持有实例 */
    uint64 Generation = 0;

private:
    friend struct FBBBCharacterNetworkDomainState;
    FBBBCharacterInteractionNetworkObservationState() = default;
};
