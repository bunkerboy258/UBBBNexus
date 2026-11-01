#include "BBBSlimePresentationActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFact.h"

ABBBSlimePresentationActor::ABBBSlimePresentationActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlimeBody"));
    SetRootComponent(Body);
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Body->SetGenerateOverlapEvents(false);
    Body->SetCanEverAffectNavigation(false);
    Body->SetCastShadow(false);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    Body->SetStaticMesh(Sphere.Object);
}

void ABBBSlimePresentationActor::ApplyFact(const FBBBSlimeFact& Fact)
{
    if (!Fact.Definition)
    {
        return;
    }
    SetActorLocation(Fact.Location + (Fact.Phase == 1 ? Fact.Normal * 2 : FVector::ZeroVector));
    SetActorRotation(FRotationMatrix::MakeFromZ(Fact.Normal).ToQuat());
    const float Scale = (Fact.Phase == 1 ? Fact.Definition->Radius : Fact.Definition->FlightRadius) / 50.0f;
    Body->SetWorldScale3D(FVector(Scale, Scale, Fact.Phase == 1 ? 0.05f : Scale));
    Body->SetMaterial(0, Fact.Definition->Material);
    const float Remaining = FMath::Max(0.1f, Fact.EndsAt - GetWorld()->GetTimeSeconds()) + 1;
    if (FMath::Abs(GetLifeSpan() - Remaining) > 0.05f)
    {
        SetLifeSpan(Remaining);
    }
}
