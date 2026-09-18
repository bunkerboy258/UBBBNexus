#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBEquipBehavior.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"

bool FBBBEquipBehavior::CanStart(const FInput &Input, const FBBBCharacterRuntimeData &Data)
{
    if (!Data.Equipment.Inventory.QuickAccessBindings.IsValidIndex(Input.EquipSlot))
    {
        return false;
    }

    ABBBEquipment *Target = Data.Equipment.Inventory.QuickAccessBindings[Input.EquipSlot];
    return IsValid(Target) && Target != Data.Equipment.Equipment.GetActiveMainHandInstance();
}

void FBBBEquipBehavior::Start(const FInput &Input, FBBBCharacterRuntimeData &Data)
{
    Data.Operation.SelectedEquipment = Data.Equipment.Inventory.QuickAccessBindings[Input.EquipSlot];
    Data.Operation.ActiveBehaviorGroups |= BBBBehaviorGroup::Equip;
    Data.Equipment.Equipment.DesiredMainHandInstance = Data.Operation.SelectedEquipment;
}
