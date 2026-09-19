#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"

bool FBBBEquipSlotPacket::IsValid() const
{
    return Slot != INDEX_NONE;
}

bool FBBBEquipSlotPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 只有权威端可以把切换请求转化为装备事实
    if (!Context.bAuthority
        || !Context.Inventory.QuickAccessBindings.IsValidIndex(Slot))
    {
        return false;
    }

    ABBBEquipment *Target = Context.Inventory.QuickAccessBindings[Slot];
    return ::IsValid(Target) && Target != Context.Equipment.GetActiveMainHandInstance();
}

void FBBBEquipSlotPacket::Apply(FBBBCharacterPacketContext &Context) const
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
