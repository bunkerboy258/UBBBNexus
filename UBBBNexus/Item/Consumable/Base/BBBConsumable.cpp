#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Config/BBBConsumableDefinition.h"
bool UBBBConsumable::InitializeUse(UBBBConsumableDefinition &InDefinition)
{
    Definition = &InDefinition;
    return GetCharacter() && InitializeRuntimeData();
}
void UBBBConsumable::UpdateUse()
{
    UpdateRuntimeData();
}
void UBBBConsumable::ShutdownUse()
{
    ShutdownRuntimeData();
    Definition = nullptr;
}
ABBBCharacter *UBBBConsumable::GetCharacter() const
{
    return GetTypedOuter<ABBBCharacter>();
}
UBBBConsumableDefinition *UBBBConsumable::GetDefinition() const
{
    return Definition;
}
