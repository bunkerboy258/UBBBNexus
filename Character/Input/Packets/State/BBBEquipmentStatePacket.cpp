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
    UBBBEquipmentDefinition *Definition = Context.Catalog.FindDefinition(EquipmentId);
    if (Definition)
    {
        Context.Commands.SubmitEquipmentState(*Definition);
    }
}
