#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/BBBEquipmentInputPipeline.h"

void FBBBEquipmentInputPipeline::Initialize(FBBBEquipmentInputRuntimeData &InInput, FBBBEquipmentEquipState &InEquip,
        FBBBEquipmentFireState &InFire, FBBBEquipmentReloadState &InReload,
        FName InEquipmentId, bool bInIsMirror)
{
    Input = &InInput;
    Equip = &InEquip;
    Fire = &InFire;
    Reload = &InReload;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentInputPipeline::Update() const
{
    // 装备输入处理需要全部运行数据有效
    if (!ensureMsgf(Input && Equip && Fire && Reload,
        TEXT("[UBBBE]Input update dependencies are invalid")))
    {
        return;
    }

    // 将待处理输入分发到各装备系统
    Processor.Update(*Input, *Equip, *Fire, *Reload, EquipmentId, bIsMirror);
}
