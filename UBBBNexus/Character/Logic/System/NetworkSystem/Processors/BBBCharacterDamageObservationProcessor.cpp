#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterDamageObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterDamageLocalControlPacket.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

void FBBBCharacterDamageObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Observation = Context.Data.Network.DamageObservationState;
    auto &Inbox = Context.Data.Network.DamageInboxState;
    auto *Target = Cast<ABBBCharacter>(Context.NetworkComponent.GetOwner());
    if (!Target)
    {
        return;
    }
    const auto &Delivery = Context.Data.Life.ReadDamageDeliveryState();
    if (Delivery.Serial > Observation.DeliverySerial)
    {
        for (int32 Index = 0; Index < Delivery.Hits.Num(); ++Index)
        {
            if (Context.NetworkIdentityState.bHasAuthority)
            {
                Context.NetworkComponent.ClientDeliverDamage(Delivery.Hits[Index]);
            }
            if (!Context.NetworkIdentityState.bHasAuthority)
            {
                APlayerController *Controller = Target->GetWorld()->GetFirstPlayerController();
                ABBBCharacter *Sender = Controller ? Cast<ABBBCharacter>(Controller->GetPawn()) : nullptr;
                if (Sender && Sender->IsLocallyControlled())
                {
                    const uint64 Sequence = (Delivery.Serial << 32) | uint64(Index + 1);
                    Sender->GetCharacterNetworkComponent()->ServerSubmitDamage(
                        Target, Delivery.Hits[Index], Sequence);
                }
                if (!Sender)
                {
                    UE_LOG(LogTemp, Warning, TEXT("角色命中无法找到本机合法连接 %s"), *Target->GetName());
                }
            }
        }
        Observation.DeliverySerial = Delivery.Serial;
    }

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        for (int32 Index = 0; Index < Inbox.Hits.Num(); ++Index)
        {
            uint64 &Last = Observation.ReceivedSequences.FindOrAdd(Inbox.Sources[Index]);
            if (Inbox.Sequences[Index] <= Last)
            {
                continue;
            }
            Last = Inbox.Sequences[Index];
            if (Context.NetworkIdentityState.bLocallyControlled)
            {
                Target->SubmitInput(FBBBCharacterDamageLocalControlPacket{{Inbox.Hits[Index]}});
            }
            if (!Context.NetworkIdentityState.bLocallyControlled)
            {
                Context.NetworkComponent.ClientDeliverDamage(Inbox.Hits[Index]);
            }
        }
        for (auto It = Observation.ReceivedSequences.CreateIterator(); It; ++It)
        {
            if (!It.Key().IsValid())
            {
                It.RemoveCurrent();
            }
        }
    }
    Inbox.Hits.Reset();
    Inbox.Sequences.Reset();
    Inbox.Sources.Reset();
}
