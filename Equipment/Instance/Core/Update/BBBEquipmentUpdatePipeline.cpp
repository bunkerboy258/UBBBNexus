#include "BBBWork/UBBBNexus/Equipment/Instance/Core/Update/BBBEquipmentUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Pipeline/Input/BBBEquipmentInputPipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Runtime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/BBBEquipmentEquipSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/BBBEquipmentReloadSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/FireSystem/BBBEquipmentFireSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/System/AnimationSystem/BBBEquipmentAnimationSystem.h"

void FBBBEquipmentUpdatePipeline::Initialize(
    FBBBEquipmentRuntimeData &InRuntimeData, FBBBEquipmentInputPipeline &InInput,
    FBBBEquipmentEquipSystem &InEquip, FBBBEquipmentReloadSystem &InReload,
    FBBBEquipmentFireSystem &InFire, FBBBEquipmentAnimationSystem &InAnimation)
{
    RuntimeData = &InRuntimeData;
    Input = &InInput;
    Equip = &InEquip;
    Reload = &InReload;
    Fire = &InFire;
    Animation = &InAnimation;
}

void FBBBEquipmentUpdatePipeline::Update() const
{
    if (!ensureMsgf(RuntimeData && Input && Equip && Reload && Fire && Animation,
        TEXT("[UBBBE]Update pipeline dependencies are invalid")))
    {
        return;
    }

    Input->Update();
    Equip->Update();
    Reload->Update();
    Fire->Update();
    Animation->Update();
    RuntimeData->CleanFrame();
}
