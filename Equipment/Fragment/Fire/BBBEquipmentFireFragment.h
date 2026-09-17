#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentFireFragment.generated.h"

struct FBBBEquipmentFireContext;
struct FBBBEquipmentRuntimeData;

/** 开火条件与执行方式的可选实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentFireFragment
{
    GENERATED_BODY()

    virtual ~FBBBEquipmentFireFragment() = default;

    /**
     * @param Runtime   当前装备事实
     * @param WorldTime 当前世界时间
     * @return 是否允许开火
     */
    virtual bool CanFire(const FBBBEquipmentRuntimeData &Runtime, float WorldTime) const
    {
        return false;
    }

    /**
     * @param Context 开火所需的装备事实
     * @return 开火行为是否成功
     */
    virtual bool Fire(FBBBEquipmentFireContext &Context) const
    {
        return false;
    }

    /** @return 枪口和瞄准来源插槽 */
    virtual FName GetMuzzleSocketName() const
    {
        return NAME_None;
    }
};
