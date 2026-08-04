
#include "BBBWork/UBBBNexus/Item/Definition/BBBItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Template/Weapon/BBBWeaponDefinition.h"
#include "BBBWork/UBBBNexus/Item/Template/Equipment/EquipmentPose/BBBEquipmentPoseFragment.h"
#include "BBBWork/UBBBNexus/Item/Template/Weapon/Fire/Base/BBBFireFragment.h"
#include "BBBWork/UBBBNexus/Item/Template/Weapon/Magazine/BBBMagazineFragment.h"

void UBBBItemRuntimeData::Initialize(const UBBBItemDefinition &Definition)
{

    RuntimeDataList.Reset();

    //按定义层级收集显式插槽
    TArray<const UBBBItemFragment *> Fragments;

    const UBBBEquipmentDefinition *EquipmentDefinition = Cast<UBBBEquipmentDefinition>(&Definition);
    if (EquipmentDefinition && EquipmentDefinition->EquipmentPoseFragment)
    {
        Fragments.Add(EquipmentDefinition->EquipmentPoseFragment);
    }

    const UBBBWeaponDefinition *WeaponDefinition = Cast<UBBBWeaponDefinition>(&Definition);
    if (WeaponDefinition && WeaponDefinition->FireFragment)
    {
        Fragments.Add(WeaponDefinition->FireFragment);
    }

    if (WeaponDefinition && WeaponDefinition->MagazineFragment)
    {
        Fragments.Add(WeaponDefinition->MagazineFragment);
    }

    //逐插槽生成运行数据 纯配置插槽返回空自动跳过
    for (const UBBBItemFragment *Fragment : Fragments)
    {

        UBBBItemFragmentRuntimeData *RuntimeData = Fragment->InitializeRuntimeData(*this);
        if (!RuntimeData)
        { continue; }

        RuntimeDataList.Add(RuntimeData);
    }
}
