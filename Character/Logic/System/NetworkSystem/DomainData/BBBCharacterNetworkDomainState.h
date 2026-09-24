#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBAimNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBRunNetworkObservationState.h"
#include "BBBCharacterNetworkDomainState.generated.h"

class FBBBAimObservationProcessor;
class FBBBRunObservationProcessor;
class FBBBCharacterNetworkSystem;

/** 角色网络领域全部观察状态的唯一直接持有者 */
USTRUCT()
struct FBBBCharacterNetworkDomainState final
{
    GENERATED_BODY()

public:
    /** @return 瞄准网络观察状态 */
    const FBBBAimNetworkObservationState &ReadAimNetworkObservationState() const
    {
        return AimObservationState;
    }

    /** @return 跑步网络观察状态 */
    const FBBBRunNetworkObservationState &ReadRunNetworkObservationState() const
    {
        return RunObservationState;
    }

private:
    friend class FBBBAimObservationProcessor;
    friend class FBBBRunObservationProcessor;
    friend class FBBBCharacterNetworkSystem;

    /** 瞄准网络观察状态 */
    UPROPERTY(Transient)
    FBBBAimNetworkObservationState AimObservationState;

    /** 跑步网络观察状态 */
    UPROPERTY(Transient)
    FBBBRunNetworkObservationState RunObservationState;
};
