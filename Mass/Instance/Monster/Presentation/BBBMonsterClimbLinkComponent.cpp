#include "BBBMonsterClimbLinkComponent.h"

#include "BBBMonsterClimbLinkActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Movement/BBBMonsterNavigationProcessor.h"

bool UBBBMonsterClimbLinkComponent::IsLinkPathfindingAllowed(const UObject* Querier) const
{
    const auto* Query = Cast<UBBBMonsterNavigationProcessor>(Querier);
    const auto* Link = Cast<ABBBMonsterClimbLinkActor>(GetOwner());
    if (!Query || !Link || !Super::IsLinkPathfindingAllowed(Querier))
    {
        return false;
    }
    const float Height = Link->GetLedge().Z - Link->GetEntry().Z;
    return Height > 0 && Height <= Query->GetTraversalLimit();
}
