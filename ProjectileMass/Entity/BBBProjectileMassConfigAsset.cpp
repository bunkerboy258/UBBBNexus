#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassConfigAsset.h"

#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassTrait.h"

UBBBProjectileMassConfigAsset::UBBBProjectileMassConfigAsset()
{
    UBBBProjectileMassTrait* ProjectileTrait = CreateDefaultSubobject<UBBBProjectileMassTrait>(TEXT("ProjectileMassTrait"));
    GetMutableConfig().AddTrait(*ProjectileTrait);
}
