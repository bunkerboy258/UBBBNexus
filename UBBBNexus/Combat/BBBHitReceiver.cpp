#include "BBBHitReceiver.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "Components/PrimitiveComponent.h"

void BBBHitReceiver::SubmitProjectileHit(AActor* Target, const FBBBHitContribution& Hit, APawn* Shooter)
{
    auto* Maodie = Cast<ABBBMaodieTest>(Target);
    if (!IsValid(Maodie))
    {
        Submit(Target, Hit);
        return;
    }
    if (!Hit.IsValid() || !IsValid(Shooter) || !Shooter->IsPlayerControlled() || !Shooter->IsLocallyControlled())
    {
        return;
    }
    if (Maodie->HasAuthority())
    {
        Maodie->SubmitInput(FBBBMaodieTestProjectileHitLocalControlPacket{{TWeakObjectPtr<APawn>(Shooter)}});
        return;
    }
    if (auto* Character = Cast<ABBBCharacter>(Shooter))
    {
        Character->GetCharacterNetworkComponent()->ServerReportMaodieProjectileHit(Maodie);
    }
}

void BBBHitReceiver::SubmitBatch(AActor* Target, TArray<FBBBHitContribution> Hits)
{
    if (!IsValid(Target) || Hits.IsEmpty())
    {
        return;
    }
    if (auto* Character = Cast<ABBBCharacter>(Target))
    {
        Character->SubmitInput(FBBBCharacterDamageLocalControlPacket{MoveTemp(Hits)});
        return;
    }

    // 原生受击对象只接收一次作用 以最后命中为接触位置并保留爆炸冲击。
    FBBBHitContribution Combined = Hits.Pop(EAllowShrinking::No);
    for (const auto& Hit : Hits)
    {
        Combined.AddDamage += Hit.AddDamage;
        Combined.AddDurableDamage += Hit.AddDurableDamage;
        Combined.AddImpulse += Hit.AddImpulse;
        Combined.AddVelocity += Hit.AddVelocity;
    }
    Submit(Target, Combined);
}

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
