#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

struct FBBBEquipmentActionFact;

/**
 * 弹匣已装填事实包 驱动解析状态机完结换弹追踪
 */
struct FBBBMagazineLoadedFactPacket final
{

    FName EquipmentId = NAME_None;

    int32 Sequence = INDEX_NONE;

    int32 LoadedAmmo = 0;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /**
     * 留档事实并完结换弹追踪
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const;

    /** @return 对应的事实记录 */
    FBBBEquipmentActionFact ToFact() const;
};
