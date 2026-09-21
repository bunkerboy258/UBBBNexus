#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBCharacterNetworkState.h"
#include "BBBCharacterNetworkDomainState.generated.h"

class FBBBAimObservationProcessor;
class FBBBEquipmentStateObservationProcessor;
class FBBBLocomotionObservationProcessor;
class FBBBCharacterNetworkSystem;

/** 角色网络观察状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterNetworkDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色网络观察状态 */
    const FBBBCharacterNetworkState &ReadNetworkState() const
    {
        return NetworkState;
    }

private:
    friend class FBBBAimObservationProcessor;
    friend class FBBBEquipmentStateObservationProcessor;
    friend class FBBBLocomotionObservationProcessor;
    friend class FBBBCharacterNetworkSystem;

    /** 角色网络观察状态 */
    UPROPERTY(Transient)
    FBBBCharacterNetworkState NetworkState;
};
