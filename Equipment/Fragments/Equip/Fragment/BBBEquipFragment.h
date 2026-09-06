#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Fragments/Equip/BBBEquipDomin.h"
#include "BBBEquipFragment.generated.h"

class ABBBEquipmentPresentationActor;
class UBBBEquipRuntimeData;
class UAnimMontage;
class USkeletalMeshComponent;

/** 装备生成、挂接与过渡领域 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipFragment final : public FBBBEquipDomin
{
    GENERATED_BODY()

public:
    /**
     * 创建装备领域运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建完成的装备领域运行数据
     */
    virtual UBBBEquipRuntimeData *InitializeRuntimeData(UObject &Outer) const override;

    /**
     * 生成并挂接装备表现实体
     * @param RuntimeData			装备领域运行数据
     * @param CharacterMesh			角色骨骼网格
     * @param CharacterAPI			角色能力接口
     * @param AttachmentSocketName	装备挂接插槽
     * @return 生成完成的装备表现实体，失败时返回空
     */
    virtual ABBBEquipmentPresentationActor *Equip(
        UBBBEquipRuntimeData &RuntimeData,
        USkeletalMeshComponent &CharacterMesh,
        FName AttachmentSocketName) const override;

    /** @return 装备动作持续时间 */
    virtual float GetEquipDuration() const override;

    /**
     * 构造一次装备动作的人物表现数据
     * @param OutPresentation 接收装备动作表现数据
     * @return 无
     */
    virtual void BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const override;

    /** 装备表现实体类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    TSubclassOf<ABBBEquipmentPresentationActor> PresentationActorClass;

    /** 装备表现实体生成偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    FTransform SpawnOffset = FTransform::Identity;

    /** 装备动作持续时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip", meta = (ClampMin = "0.01"))
    float EquipDuration = 0.8f;

    /** 装备人物动作蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    /** 瞄准来源插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName AimSourceSocketName = TEXT("Muzzle");

    /** 左手 IK 目标插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName LeftHandGripSocketName = NAME_None;

    /** 左手 IK 目标相对 Socket 的武器坐标空间位置偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FVector LeftHandGripSocketOffset = FVector::ZeroVector;

    /** 左手 IK 目标是否每帧重读局部修正，仅用于调试 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    bool bRefreshLeftHandGripSocketOffsetEveryFrame = false;

};
