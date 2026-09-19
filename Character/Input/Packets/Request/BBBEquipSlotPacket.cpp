#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"

bool FBBBEquipSlotPacket::IsValid() const
{
    return Slot != INDEX_NONE;
}

bool FBBBEquipSlotPacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // 还原模式不接受本地请求 空槽 无效槽与当前装备同槽都不构成切换
    if (Context.Operation.IsRestoreMode()
        || !Context.Inventory.QuickAccessBindings.IsValidIndex(Slot))
    {
        return false;
    }

    ABBBEquipment *Target = Context.Inventory.QuickAccessBindings[Slot];
    return ::IsValid(Target) && Target != Context.Equipment.GetActiveMainHandInstance();
}

void FBBBEquipSlotPacket::Execute(FBBBCharacterPacketContext &Context) const
{
    // 切换装备打断进行中的换弹
    if (Context.Operation.IsReloadInProgress())
    {
        Context.Operation.CancelReload();
    }

    ABBBEquipment *Target = Context.Inventory.QuickAccessBindings[Slot];
    Context.Operation.SelectEquipment(*Target);
    Context.Equipment.SetDesiredMainHandInstance(Target);
}
