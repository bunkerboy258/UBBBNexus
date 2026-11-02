#include "BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "Components/PrimitiveComponent.h"

void UBBBPhysicsHitReceiver::ReceiveHit(const FBBBHitContribution& Hit)
{
    if (!Hit.IsValid())
    {
        return;
    }
    TInlineComponentArray<UPrimitiveComponent*> Bodies(GetOwner());
    for (auto* Body : Bodies)
    {
        if (!Body->IsSimulatingPhysics())
        {
            continue;
        }
        if (!Hit.AddVelocity.IsNearlyZero())
        {
            Body->AddImpulse(Hit.AddVelocity, NAME_None, true);
        }
        if (!Hit.AddImpulse.IsNearlyZero())
        {
            Body->AddImpulse(Hit.AddImpulse);
        }
    }
}

void BBBHitReceiver::Submit(AActor* Target, const FBBBHitContribution& Hit)
{
    if (!IsValid(Target) || !Hit.IsValid())
    {
        return;
    }
    if (auto* Character = Cast<ABBBCharacter>(Target))
    {
        Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{{Hit}});
        return;
    }
    if (auto* Receiver = Cast<IBBBHitReceiver>(Target))
    {
        Receiver->ReceiveHit(Hit);
        return;
    }
    if (auto* Throwable = Cast<ABBBThrowable>(Target))
    {
        Throwable->SubmitInput(FBBBThrowableHitLocalControlPacket{{Hit}});
        return;
    }
    if (Cast<ABBBEquipment>(Target))
    {
        return;
    }
    if (Hit.AddImpulse.IsNearlyZero() && Hit.AddVelocity.IsNearlyZero())
    {
        return;
    }
    auto* Physics = Target->FindComponentByClass<UBBBPhysicsHitReceiver>();
    if (!Physics)
    {
        TInlineComponentArray<UPrimitiveComponent*> Bodies(Target);
        if (!Bodies.ContainsByPredicate([](const auto* Body)
        {
            return Body->IsSimulatingPhysics();
        }))
        {
            return;
        }
        Physics = NewObject<UBBBPhysicsHitReceiver>(Target);
        Target->AddInstanceComponent(Physics);
        Physics->RegisterComponent();
    }
    Physics->ReceiveHit(Hit);
}
