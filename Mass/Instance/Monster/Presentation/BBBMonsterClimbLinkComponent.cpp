#include "BBBMonsterClimbLinkComponent.h"

#include "BBBMonsterClimbLinkActor.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassNavigationQueryActor.h"

bool UBBBMonsterClimbLinkComponent::IsLinkPathfindingAllowed(const UObject* Querier) const
{
    const auto* Query = Cast<ABBBMassNavigationQueryActor>(Querier);
    const auto* Link = Cast<ABBBMonsterClimbLinkActor>(GetOwner());
    if (!Query || !Link || !Super::IsLinkPathfindingAllowed(Querier))
    {
        return false;
    }
    const float Height = Link->GetExit().Z - Link->GetEntry().Z;
    return Height > 0 && Height <= Query->GetTraversalLimit();
}
