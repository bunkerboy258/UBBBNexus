#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Processors/BBBCharacterInputProcessor.h"

class FBBBCharacterInitializer;
class UBBBEquipmentCatalog;
struct FBBBCharacterRuntimeData;

/** 角色包解析系统 负责将输入转换为领域黑板事实 */
class ABBB_EVAC_API FBBBCharacterParseSystem final
{
public:
    /**
     * 解析当前固定输入帧
     * @param bAuthority          当前实例是否拥有玩法权威
     * @param bLocallyControlled 当前实例是否由本机控制
     */
    void Update(bool bAuthority, bool bLocallyControlled) const;

private:
    friend class FBBBCharacterInitializer;

    void Initialize(FBBBCharacterRuntimeData &InData, UBBBEquipmentCatalog &InEquipmentCatalog);

    FBBBCharacterRuntimeData *Data = nullptr;
    UBBBEquipmentCatalog *EquipmentCatalog = nullptr;
    FBBBCharacterInputProcessor InputProcessor;
};
