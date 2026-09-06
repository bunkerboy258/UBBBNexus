#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Magazine/BBBMagazineDomin.h"
#include "BBBMagazineFragment.generated.h"

class ABBBEquipmentPresentationActor;
class ABBBMagazinePresentationActor;
class UBBBMagazineRuntimeData;
class UAnimMontage;

/** 弹匣与换弹领域 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMagazineFragment final : public FBBBMagazineDomin
{
    GENERATED_BODY()

public:
    /**
     * 创建并初始化弹匣运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建完成的弹匣运行数据
     */
    virtual UBBBMagazineRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    /**
     * 判断当前弹匣是否允许开火
     * @param RuntimeData	弹匣运行数据
     * @return 是否允许消耗弹药
     */
    virtual bool CanConsumeRound(const UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 消耗一发弹药
     * @param RuntimeData	弹匣运行数据
     */
    virtual void ConsumeRound(UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 响应一次本地换弹
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		弹匣运行数据
     * @return 是否成功开始换弹
     */
    virtual bool CanStartReload(const UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 响应一次远端换弹表现
     * @param CharacterAPI	角色能力接口
     */
    virtual void CommitReload(UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 更新换弹过程
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		弹匣运行数据
     */
    /** @return 换弹动作持续时间 */
    virtual float GetReloadDuration() const override;

    /**
     * 构造一次换弹动作的人物表现数据
     * @param OutPresentation 接收换弹动作表现数据
     * @return 无
     */
    virtual void BuildReloadActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const override;

    /** @return 弹匣卸下归一化时刻 */
    virtual float GetMagazineRemoveNormalizedTime() const override;

    /** @return 弹匣生成归一化时刻 */
    virtual float GetMagazineSpawnNormalizedTime() const override;

    /**
     * 生成并装入弹匣表现实体
     * @param PresentationActor    装备表现实体
     * @param RuntimeData          弹匣运行数据
     * @return 是否成功生成或已经存在弹匣
     */
    virtual bool SpawnMagazine(
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 拔出并掉落当前弹匣表现实体
     * @param RuntimeData    弹匣运行数据
     */
    virtual void RemoveMagazine(UBBBMagazineRuntimeData &RuntimeData) const override;

    /**
     * 销毁仍然安装在武器上的弹匣表现实体
     * @param RuntimeData    弹匣运行数据
     */
    virtual void DestroyLoadedMagazine(UBBBMagazineRuntimeData &RuntimeData) const override;

    /** 弹匣容量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "1"))
    int32 MagazineSize = 30;

    /** 初始备用弹药 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 90;

    /** 换弹动作持续时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0.01"))
    float ReloadDuration = 2.2f;

    /** 换弹人物动作蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine|Animation")
    TObjectPtr<UAnimMontage> ReloadMontage = nullptr;

    /** 弹匣卸下归一化时刻 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MagazineRemoveNormalizedTime = 0.35f;

    /** 弹匣生成归一化时刻 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MagazineSpawnNormalizedTime = 0.75f;

    /** 弹匣表现实体类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    TSubclassOf<ABBBMagazinePresentationActor> MagazinePresentationActorClass;

    /** 弹匣在装备网格上的挂接插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    FName MagazineSocketName = TEXT("Magazine");

    /** 弹匣相对 Socket 的武器坐标空间位置偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    FVector MagazineSocketOffset = FVector::ZeroVector;

    /** 弹匣是否每帧重读局部修正，仅用于调试 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine")
    bool bRefreshMagazineSocketOffsetEveryFrame = false;

    /** 掉落弹匣自动销毁前的保留时长 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Magazine", meta = (ClampMin = "0.1"))
    float DroppedMagazineLifeSeconds = 5.0f;
};
