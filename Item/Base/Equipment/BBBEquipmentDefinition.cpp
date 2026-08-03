
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Fragments/EquipmentPose/BBBEquipmentPoseFragment.h"

//收集装备层 Fragment 命名插槽
void UBBBEquipmentDefinition::CollectFragments(TArray<const UBBBItemFragment *> &OutFragments) const
{
    Super::CollectFragments(OutFragments);

    if (EquipmentPoseFragment)
    {
        OutFragments.Add(EquipmentPoseFragment);
    }
}
