
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Fragment/BBBItemFragment.h"
#include "BBBFireFragment.generated.h"
class ABBBWeaponActor;
class UBBBFireRuntimeData;
class UBBBItemFragmentRuntimeData;
struct FBBBShotEvent;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBFireFragment : public UBBBItemFragment
{
    GENERATED_BODY()
public:

    /**
     * 创建开火运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建的开火运行数据
     */
    virtual UBBBItemFragmentRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    /**
     * 执行开火逻辑 纯虚函数由子类实现
     * @param Weapon	武器模型
     * @param RuntimeData	开火运行数据
     * @param OutShotEvent	输出射击事件
     * @return 是否成功开火
     */
    virtual bool Fire(
        ABBBWeaponActor &Weapon,
        UBBBFireRuntimeData &RuntimeData,
        FBBBShotEvent &OutShotEvent) const PURE_VIRTUAL(UBBBFireFragment::Fire, return false;);

    /**
     * 播放开火表现 纯虚函数由子类实现
     * @param Weapon	武器模型
     */
    virtual void Present(
        ABBBWeaponActor &Weapon) const PURE_VIRTUAL(UBBBFireFragment::Present,);
};
