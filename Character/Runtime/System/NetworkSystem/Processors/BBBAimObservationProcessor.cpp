#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBAimObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Network/BBBNetworkConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/DomainData/States/BBBAimState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBCharacterNetworkState.h"

void FBBBAimObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    FBBBAimNetworkPayload Payload;
    Payload.bIsAiming = Context.AimState.bIsAiming;
    Payload.AimTargetWorld = Context.AimState.AimTargetWorld;

    bool bShouldTransmit = !Context.NetworkState.LastObservedAim.IsSet();
    if (Context.NetworkState.LastObservedAim.IsSet())
    {
        const FBBBAimNetworkPayload &Previous = Context.NetworkState.LastObservedAim.GetValue();
        bShouldTransmit = Previous.bIsAiming != Payload.bIsAiming;

        const bool bIntervalElapsed = Context.WorldState.WorldTimeSeconds
            - Context.NetworkState.LastAimUploadTime
            >= Context.NetworkConfig.AimUploadInterval;
        const bool bTargetChanged = !FVector(Previous.AimTargetWorld).Equals(
            FVector(Payload.AimTargetWorld),
            0.5f);
        bShouldTransmit |= bIntervalElapsed && bTargetChanged;
    }

    if (!bShouldTransmit)
    {
        return;
    }

    Context.NetworkState.LastObservedAim = Payload;
    Context.NetworkState.LastAimUploadTime = Context.WorldState.WorldTimeSeconds;

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateAimState(Payload);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitAimState(Payload);
    }
}
