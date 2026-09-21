#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBAimNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBEquipmentNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBLocomotionNetworkObservationState.h"
#include "BBBCharacterNetworkDomainState.generated.h"

class FBBBAimObservationProcessor;
class FBBBEquipmentStateObservationProcessor;
class FBBBLocomotionObservationProcessor;
class FBBBCharacterNetworkSystem;

/** 角色网络领域全部观察状态的唯一直接持有者 */
USTRUCT()
struct FBBBCharacterNetworkDomainState final
{
    GENERATED_BODY()

public:
    /** @return 装备网络观察状态 */
    const FBBBEquipmentNetworkObservationState &ReadEquipmentNetworkObservationState() const
    {
        return EquipmentObservationState;
    }

    /** @return 瞄准网络观察状态 */
    const FBBBAimNetworkObservationState &ReadAimNetworkObservationState() const
    {
        return AimObservationState;
    }

    /** @return 移动网络观察状态 */
    const FBBBLocomotionNetworkObservationState &ReadLocomotionNetworkObservationState() const
    {
        return LocomotionObservationState;
    }

private:
    friend class FBBBAimObservationProcessor;
    friend class FBBBEquipmentStateObservationProcessor;
    friend class FBBBLocomotionObservationProcessor;
    friend class FBBBCharacterNetworkSystem;

    /** 装备网络观察状态 */
    UPROPERTY(Transient)
    FBBBEquipmentNetworkObservationState EquipmentObservationState;

    /** 瞄准网络观察状态 */
    UPROPERTY(Transient)
    FBBBAimNetworkObservationState AimObservationState;

    /** 移动网络观察状态 */
    UPROPERTY(Transient)
    FBBBLocomotionNetworkObservationState LocomotionObservationState;
};
