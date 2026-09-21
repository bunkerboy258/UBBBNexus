#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentStateObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/States/BBBCharacterNetworkState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

void FBBBEquipmentStateObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    ABBBEquipment *ActiveEquipment = Context.EquipmentSelectionState.ActiveMainHandInstance;
    if (!ActiveEquipment)
    {
        return;
    }

    if (Context.NetworkState.LastUploadedEquipmentInstanceId == ActiveEquipment->GetInstanceId())
    {
        return;
    }

    const FName EquipmentId = ActiveEquipment->GetEquipmentId();
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateEquipmentState(EquipmentId);
    }

    if (!Context.NetworkIdentityState.bHasAuthority
        && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitEquipmentState(EquipmentId);
    }

    Context.NetworkState.LastUploadedEquipmentInstanceId = ActiveEquipment->GetInstanceId();
}
