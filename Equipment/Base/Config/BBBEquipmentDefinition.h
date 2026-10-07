#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Config/BBBEquipmentType.h"
#include "BBBEquipmentDefinition.generated.h"

class UBBBEquipmentAnimInstance;
class UAnimInstance;
class USkeletalMesh;
class UTexture2D;

/** 所有装备实例共享的静态资源配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** 装备所属类别 供角色动画读取统一分类 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (DisplayName = "装备类型"))
    EBBBEquipmentType EquipmentType = EBBBEquipmentType::None;

    /** 装备在目录查找和网络同步中使用的标识 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (DisplayName = "装备标识"))
    FName EquipmentId;

    /** 装备显示名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (DisplayName = "显示名称"))
    FText DisplayName;

    /** 装备说明文本 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (DisplayName = "说明"))
    FText Description;

    /** 装备显示图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备", meta = (DisplayName = "图标"))
    TObjectPtr<UTexture2D> Icon;

    /** 角色装备后链接的动画层类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|动画", meta = (DisplayName = "角色动画层类"))
    TSubclassOf<UAnimInstance> CharacterAnimationLayerClass;

    /** 装备骨骼网格使用的动画实例类型 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|动画", meta = (DisplayName = "装备动画类"))
    TSubclassOf<UBBBEquipmentAnimInstance> EquipmentAnimationClass;

    /** 装备使用的骨骼网格 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|外观", meta = (DisplayName = "装备骨骼网格"))
    TObjectPtr<USkeletalMesh> EquipmentMesh = nullptr;

    /** 装备挂接到角色后的相对变换 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|装备|外观", meta = (DisplayName = "生成偏移"))
    FTransform SpawnOffset = FTransform::Identity;

};
