#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBEquipBehavior.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Character/Input/Behaviors/BBBReloadBehavior.h"

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
    if (Data.Operation.ReloadSequence > 0)
    {
        FBBBReloadBehavior::Cancel(Data);
    }
    Data.Operation.SelectedEquipment = Data.Equipment.Inventory.QuickAccessBindings[Input.EquipSlot];
    Data.Equipment.Equipment.DesiredMainHandInstance = Data.Operation.SelectedEquipment;
}
