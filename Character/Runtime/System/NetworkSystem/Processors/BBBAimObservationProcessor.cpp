#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBAimNetworkObservationState.h"

void FBBBAimObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    FBBBReplicatedAimState ReplicatedAimState;
    ReplicatedAimState.bIsAiming = Context.AimState.bIsAiming;
    ReplicatedAimState.AimTargetWorld = Context.AimState.AimTargetWorld;

    bool bShouldTransmit = !Context.AimObservationState.LastObservedAim.IsSet();
    if (Context.AimObservationState.LastObservedAim.IsSet())
    {
        const FBBBReplicatedAimState &Previous = Context.AimObservationState.LastObservedAim.GetValue();
        bShouldTransmit = Previous.bIsAiming != ReplicatedAimState.bIsAiming;

        const bool bIntervalElapsed = Context.WorldState.WorldTimeSeconds
            - Context.AimObservationState.LastAimUploadTime
            >= Context.NetworkConfig.AimUploadInterval;
        const bool bTargetChanged = !FVector(Previous.AimTargetWorld).Equals(
            FVector(ReplicatedAimState.AimTargetWorld),
            0.5f);
        bShouldTransmit |= bIntervalElapsed && bTargetChanged;
    }

    if (!bShouldTransmit)
    {
        return;
    }

    Context.AimObservationState.LastObservedAim = ReplicatedAimState;
    Context.AimObservationState.LastAimUploadTime = Context.WorldState.WorldTimeSeconds;

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateAimState(ReplicatedAimState);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitAimState(ReplicatedAimState);
    }
}
