#include "BBBWork/UBBBNexus/Character/Input/Packets/Replication/BBBRestoreEquipmentPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

bool FBBBRestoreEquipmentPacket::IsValid() const
{
    return EquipmentHandle != NAME_None;
}

bool FBBBRestoreEquipmentPacket::CanApply(const FBBBCharacterPacketContext &Context) const
{
    // 还原包只在还原模式生效 本地角色的装备选择由请求带决定
    return Context.Operation.IsRestoreMode();
}

void FBBBRestoreEquipmentPacket::Apply(FBBBCharacterPacketContext &Context) const
{
    UBBBEquipmentDefinition *Definition = Context.Catalog.FindDefinition(EquipmentHandle);
    if (ensureMsgf(Definition, TEXT("[UBBBC]Unknown equipment restore handle %s"), *EquipmentHandle.ToString()))
    {
        Context.Commands.SubmitRestoredEquipment(*Definition);
    }
}
