#include "BBBMonsterClimbLinkActor.h"

#include "NavLinkCustomComponent.h"
#include "BBBMonsterClimbLinkComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterClimbArea.h"

ABBBMonsterClimbLinkActor::ABBBMonsterClimbLinkActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UBBBMonsterClimbLinkComponent>(TEXT("SmartLinkComp")))
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    PointLinks.Reset();
    bSmartLinkIsRelevant = true;
    GetSmartLinkComp()->SetEnabledArea(UBBBMonsterClimbArea::StaticClass());
}

void ABBBMonsterClimbLinkActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    PointLinks.Reset();
    GetSmartLinkComp()->SetLinkData(Entry, Exit, ENavLinkDirection::LeftToRight);
    SetSmartLinkEnabled(true);
}
