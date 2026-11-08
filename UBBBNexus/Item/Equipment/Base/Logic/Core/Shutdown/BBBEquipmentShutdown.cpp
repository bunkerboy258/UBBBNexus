#include "BBBWork/UBBBNexus/Item/Equipment/Base/Logic/Core/Shutdown/BBBEquipmentShutdown.h"

#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"

void FBBBEquipmentShutdown::Shutdown(ABBBEquipment &Equipment) const
{
    ShutdownInstance(Equipment);
    Equipment.SetActorTickEnabled(false);
}
