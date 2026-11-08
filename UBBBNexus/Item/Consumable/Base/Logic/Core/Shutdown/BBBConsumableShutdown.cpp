#include "BBBWork/UBBBNexus/Item/Consumable/Base/Logic/Core/Shutdown/BBBConsumableShutdown.h"
void FBBBConsumableShutdown::Shutdown(UBBBConsumable &Item) const
{
    ShutdownInstance(Item);
}
