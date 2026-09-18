#include "BBBWork/UBBBNexus/Character/Input/Events/BBBEquipmentActionEvent.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBEquipmentActionEvent::IsValid() const
{
    return Sequence > 0;
}

void FBBBEquipmentActionEvent::Record(FBBBCharacterRuntimeData &Data) const
{
    Data.Equipment.Events.AddAction(*this);
}
