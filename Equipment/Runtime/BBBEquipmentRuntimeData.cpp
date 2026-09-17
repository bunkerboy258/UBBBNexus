#include "BBBWork/UBBBNexus/Equipment/Runtime/BBBEquipmentRuntimeData.h"

void FBBBEquipmentRuntimeData::CleanFrame()
{
    Equip.Inputs.Reset();
    Fire.Inputs.Reset();
    Reload.Inputs.Reset();
}
