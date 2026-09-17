#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterRestoreInput.h"
struct FBBBNetworkRuntimeData;
class UBBBEquipmentCatalog;

/** 将传输格式转换为角色还原输入 */
class FBBBCharacterNetworkRestorer final
{
public:
    /**
     * 取出当前网络批次并转换数据
     * @param NetworkData	网络收件箱
     * @param Catalog	装备定义目录
     * @return 角色还原输入
     */
    FBBBCharacterRestoreInput Build(FBBBNetworkRuntimeData &NetworkData, UBBBEquipmentCatalog &Catalog);

private:
    int32 LastActionSequence = INDEX_NONE;
    int32 LastActionPhase = INDEX_NONE;
};
