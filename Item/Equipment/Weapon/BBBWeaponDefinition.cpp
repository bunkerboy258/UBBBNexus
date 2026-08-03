
#include "BBBWork/UBBBNexus/Item/Equipment/Weapon/BBBWeaponDefinition.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Fire/Base/BBBFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Fragments/Magazine/BBBMagazineFragment.h"

//收集武器层 Fragment 命名插槽
void UBBBWeaponDefinition::CollectFragments(TArray<const UBBBItemFragment *> &OutFragments) const
{
    Super::CollectFragments(OutFragments);

    if (FireFragment)
    {
        OutFragments.Add(FireFragment);
    }

    if (MagazineFragment)
    {
        OutFragments.Add(MagazineFragment);
    }
}
