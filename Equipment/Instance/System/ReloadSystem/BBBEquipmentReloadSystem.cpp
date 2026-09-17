#include "BBBWork/UBBBNexus/Equipment/Instance/System/ReloadSystem/BBBEquipmentReloadSystem.h"

void FBBBEquipmentReloadSystem::Initialize(FBBBEquipmentReloadRuntimeData &InData, FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentReloadFragment &InFragment, FBBBCharacterExternalAPI &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror)
{
    Data = &InData;
    Fire = &InFire;
    Fragment = &InFragment;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentReloadSystem::Update() const
{
    // 换弹更新需要运行数据和角色接口有效
    if (!ensureMsgf(Data && Fire && Fragment && CharacterAPI,
        TEXT("[UBBBE]Reload update dependencies are invalid")))
    {
        return;
    }

    // 处理换弹阶段并同步换弹事件
    Processor.Update(*Data, *Fire, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
