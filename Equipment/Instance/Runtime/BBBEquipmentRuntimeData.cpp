#include "BBBWork/UBBBNexus/Equipment/Instance/Runtime/BBBEquipmentRuntimeData.h"

void FBBBEquipmentRuntimeData::CleanFrame()
{
    // 清理本帧已经消费的装备输入
    Equip.Inputs.Reset();
    Fire.Inputs.Reset();
    Reload.Inputs.Reset();
}
