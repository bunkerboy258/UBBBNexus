#include "BBBWork/UBBBNexus/Equipment/Instance/System/EquipSystem/BBBEquipmentEquipSystem.h"

void FBBBEquipmentEquipSystem::Initialize(FBBBEquipmentEquipRuntimeData &InData, const FBBBEquipmentFireRuntimeData &InFire,
        const FBBBEquipmentEquipFragment &InFragment, ABBBCharacter &InCharacterAPI,
        FName InEquipmentId, bool bInIsMirror)
{
    Data = &InData;
    Fire = &InFire;
    Fragment = &InFragment;
    CharacterAPI = &InCharacterAPI;
    EquipmentId = InEquipmentId;
    bIsMirror = bInIsMirror;
}

void FBBBEquipmentEquipSystem::Update() const
{
    // 装备切换更新需要运行数据和角色接口有效
    if (!ensureMsgf(Data && Fire && Fragment && CharacterAPI,
        TEXT("[UBBBE]Equip update dependencies are invalid")))
    {
        return;
    }

    // 处理装备切换并发布表现事件
    Processor.Update(*Data, *Fire, *Fragment, *CharacterAPI, EquipmentId, bIsMirror);
}
