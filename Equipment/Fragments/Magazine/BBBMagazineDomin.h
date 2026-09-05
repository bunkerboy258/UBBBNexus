#pragma once

#include "CoreMinimal.h"
#include "BBBMagazineDomin.generated.h"

class ABBBEquipmentPresentationActor;
class ABBBMagazinePresentationActor;
class UBBBMagazineRuntimeData;

/** 弹匣操作域基类 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMagazineDomin
{
    GENERATED_BODY()

public:
    virtual ~FBBBMagazineDomin() = default;

    /**
     * 创建弹匣操作域运行数据
     * @param Outer	运行数据所有者
     * @return 创建完成的运行数据
     */
    virtual UBBBMagazineRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 判断是否允许消耗弹药
     * @param RuntimeData	弹匣操作域运行数据
     * @return 是否允许消耗弹药
     */
    virtual bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 消耗一发弹药
     * @param RuntimeData	弹匣操作域运行数据
     */
    virtual void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 执行一次换弹
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData			弹匣操作域运行数据
     * @return 是否成功开始换弹
     */
    virtual bool CanStartReload(const UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 播放一次换弹表现
     * @param CharacterAPI	角色能力接口
     */
    virtual void CommitReload(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 更新弹匣操作域
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData			弹匣操作域运行数据
     */
    /** @return 换弹动作持续时间 */
    virtual float GetReloadDuration() const;

    /** @return 弹匣卸下归一化时刻 */
    virtual float GetMagazineRemoveNormalizedTime() const;

    /** @return 弹匣生成归一化时刻 */
    virtual float GetMagazineSpawnNormalizedTime() const;

    /**
     * 生成并装入弹匣表现实体
     * @param PresentationActor    装备表现实体
     * @param RuntimeData          弹匣运行数据
     * @return 是否成功生成或已经存在弹匣
     */
    virtual bool SpawnMagazine(
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 应用弹匣挂接的运行时偏移
     * @param RuntimeData	弹匣运行数据
     */
    virtual void ApplyRuntimeSocketOffset(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 拔出并掉落当前弹匣表现实体
     * @param RuntimeData    弹匣运行数据
     */
    virtual void RemoveMagazine(UBBBMagazineRuntimeData &RuntimeData) const;

    /**
     * 销毁仍然安装在武器上的弹匣表现实体
     * @param RuntimeData    弹匣运行数据
     */
    virtual void DestroyLoadedMagazine(UBBBMagazineRuntimeData &RuntimeData) const;
};
