#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/BBBThrowable.h"
UBBBThrowableNetworkComponent::UBBBThrowableNetworkComponent()
{
    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = false;
}
void UBBBThrowableNetworkComponent::MulticastExplosion_Implementation(const FBBBThrowableExplosionFact& Fact)
{
    if (auto* Actor = Cast<ABBBThrowable>(GetOwner()))
    {
        Actor->SubmitInput(FBBBThrowableExplosionAuthorityFactPacket{Fact});
    }
}

#include "Net/UnrealNetwork.h"
void UBBBThrowableNetworkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBBBThrowableNetworkComponent, Explosion);
}
void UBBBThrowableNetworkComponent::Publish(const FBBBThrowableExplosionFact& Fact)
{
    Explosion = Fact;
    MulticastExplosion(Fact);
}
void UBBBThrowableNetworkComponent::OnRep_Explosion()
{
    if (auto* Actor = Cast<ABBBThrowable>(GetOwner()))
    {
        Actor->SubmitInput(FBBBThrowableExplosionAuthorityFactPacket{Explosion});
    }
}
