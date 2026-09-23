#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterOperation.h"
#include "BBBWork/UBBBNexus/Equipment/Template/BBBEquipment.h"

bool FBBBEquipSlotPacket::IsValid() const
{
    return Slot != INDEX_NONE;
}

bool FBBBEquipSlotPacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // 快捷槽位必须存在有效装备且不能与当前主手相同
    if (!Context.Inventory.ItemBarSlots.IsValidIndex(Slot))
    {
        return false;
    }

    ABBBEquipment *Target = Cast<ABBBEquipment>(Context.Inventory.ItemBarSlots[Slot].ItemActor.Get());
    return ::IsValid(Target) && Target != Context.Equipment.ActiveMainHandInstance;
}

void FBBBEquipSlotPacket::Apply(FBBBCharacterInputContext &Context) const
{
    // 切换装备打断进行中的换弹
    if (BBBCharacterOperation::IsReloadInProgress(Context.Operation))
    {
        BBBCharacterOperation::CancelReload(Context.Operation);
    }

    ABBBEquipment *Target = Cast<ABBBEquipment>(Context.Inventory.ItemBarSlots[Slot].ItemActor.Get());
    Context.Operation.SelectedEquipment = Target;
    Context.Equipment.DesiredMainHandInstance = Target;
}
