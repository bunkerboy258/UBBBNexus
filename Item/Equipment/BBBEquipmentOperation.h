#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/Operation/BBBItemOperation.h"
#include "BBBEquipmentOperation.generated.h"

class ABBBEquipmentActor;
class FBBBCharacterExternalAPI;
class UAnimMontage;
class UBBBItemInstance;
class USkeletalMeshComponent;

/** 可装备操作域 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class ABBB_EVAC_API UBBBEquipmentOperation final : public UBBBItemOperation
{
    GENERATED_BODY()

public:
    /**
     * 生成并装备物品实体
     * @param ItemInstance			所属物品实例
     * @param CharacterMesh			角色骨骼网格
     * @param CharacterAPI			角色能力接口
     * @param AttachmentSocketName	装备挂接插槽
     * @return 生成完成的装备实体，失败时返回空
     */
    ABBBEquipmentActor *Equip(
        UBBBItemInstance &ItemInstance,
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName) const;

    /** 装备实体类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    TSubclassOf<ABBBEquipmentActor> EquipmentActorClass;

    /** 装备实体生成偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment")
    FTransform SpawnOffset = FTransform::Identity;

    /** 装备过渡动画 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment|Animation")
    TObjectPtr<UAnimMontage> EquipMontage = nullptr;

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
