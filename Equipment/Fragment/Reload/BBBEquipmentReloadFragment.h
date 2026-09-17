#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentReloadFragment.generated.h"

struct FBBBEquipmentReloadContext;
struct FBBBEquipmentRuntimeData;
struct FBBBEquipmentAmmoConfig;

/** 换弹阶段处理方式的可选实现 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentReloadFragment
{
    GENERATED_BODY()

    virtual ~FBBBEquipmentReloadFragment() = default;

    /**
     * @param Runtime    当前装备事实
     * @param AmmoConfig 弹药容量配置
     * @return 是否允许开始换弹
     */
    virtual bool CanReload(const FBBBEquipmentRuntimeData &Runtime, const FBBBEquipmentAmmoConfig &AmmoConfig) const
    {
        return false;
    }

    /**
     * @param Context 换弹所需的装备事实
     * @return 换弹表现是否已提交
     */
    virtual bool Begin(FBBBEquipmentReloadContext &Context) const
    {
        return false;
    }

    /**
     * @param Context 换弹所需的装备事实
     * @return 是否成功卸下弹夹
     */
    virtual bool DetachMagazine(FBBBEquipmentReloadContext &Context) const
    {
        return false;
    }

    /**
     * @param Context 换弹所需的装备事实
     * @return 是否成功装填弹夹
     */
    virtual bool LoadMagazine(FBBBEquipmentReloadContext &Context) const
    {
        return false;
    }

    /**
     * @param Context 换弹所需的装备事实
     * @return 是否成功取消换弹
     */
    virtual bool Cancel(FBBBEquipmentReloadContext &Context) const
    {
        return false;
    }
};
