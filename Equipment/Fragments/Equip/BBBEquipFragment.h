#pragma once

#include "CoreMinimal.h"
#include "BBBEquipFragment.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UAnimMontage;
class UBBBEquipRuntimeData;
class UCurveFloat;
class USkeletalMeshComponent;

/** 装备生成、挂接与过渡领域 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipFragment
{
    GENERATED_BODY()

public:
    /**
     * 创建装备领域运行数据
     * @param Outer	运行数据生命周期所有者
     * @return 创建完成的装备领域运行数据
     */
    UBBBEquipRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 生成并挂接装备表现实体
     * @param RuntimeData			装备领域运行数据
     * @param CharacterMesh			角色骨骼网格
     * @param CharacterAPI			角色能力接口
     * @param AttachmentSocketName	装备挂接插槽
     * @return 生成完成的装备表现实体，失败时返回空
     */
    ABBBEquipmentPresentationActor *Equip(
        UBBBEquipRuntimeData &RuntimeData,
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName) const;

    /**
     * 更新装备过渡
     * @param PresentationActor	装备表现实体
     * @param RuntimeData		装备领域运行数据
     */
    void Update(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBEquipRuntimeData &RuntimeData) const;

    /** 装备表现实体类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    TSubclassOf<ABBBEquipmentPresentationActor> PresentationActorClass;

    /** 装备表现实体生成偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    FTransform SpawnOffset = FTransform::Identity;

    /** 装备过渡动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

    /** 装备期间左手IK权重曲线，横轴为归一化装备进度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Equip")
    TObjectPtr<UCurveFloat> EquipLeftHandIKAlphaCurve = nullptr;

    /** 瞄准来源插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName AimSourceSocketName = TEXT("Muzzle");

    /** 左手握持插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FName LeftHandGripSocketName = TEXT("LeftHand");

    /** 左手握持插槽修正 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    FTransform LeftHandGripSocketLocalOffset = FTransform::Identity;

    /** 是否启用左手逆向动力学 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Pose")
    bool bEnableLeftHandIK = true;
};
