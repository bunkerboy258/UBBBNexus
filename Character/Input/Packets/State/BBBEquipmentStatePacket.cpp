#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBEquipmentStatePacket.h"

#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

bool FBBBEquipmentStatePacket::IsValid() const
{
    return EquipmentId != NAME_None;
}

bool FBBBEquipmentStatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    return true;
}

void FBBBEquipmentStatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    TSubclassOf<ABBBEquipment> EquipmentClass = Context.Catalog.FindEquipmentClass(EquipmentId);
    if (ensureMsgf(
        EquipmentClass,
        TEXT("装备状态输入无法找到标识 %s 对应的 Actor 类"),
        *EquipmentId.ToString()))
    {
        Context.Commands.PendingEquipmentClass = EquipmentClass;
    }
}
