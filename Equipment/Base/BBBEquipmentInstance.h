    #pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentInstance.generated.h"

class ABBBEquipmentPresentationActor;
class UBBBEquipmentDefinition;
class UBBBEquipmentRuntimeData;
class UBBBEquipmentSystem;

/** 装备配置、运行数据与表现实体的唯一实例根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentInstance final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 创建完整装备实例
     * @param Outer			实例生命周期所有者
     * @param InDefinition	装备静态配置
     * @return 创建完成的装备实例
     */
    static UBBBEquipmentInstance *Create(UObject &Outer, UBBBEquipmentDefinition &InDefinition);

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const;

    /** @return 单件装备行为系统 */
    UBBBEquipmentSystem *GetEquipmentSystem() const;

    /** @return 当前装备表现实体 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    ABBBEquipmentPresentationActor *GetPresentationActor() const
    {
        return PresentationActor;
    }

    /** @return 装备初始化时缓存的左手 IK 相对右手骨骼位置 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector GetLeftHandIKOffsetRightHand() const
    {
        return LeftHandIKOffsetRightHand;
    }

    /** @return 装备初始化时缓存的左手 IK 位置是否有效 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool HasValidLeftHandIKOffset() const
    {
        return bHasValidLeftHandIKOffset;
    }

private:
    friend class UBBBEquipmentSystem;

    void SetLeftHandIKOffsetRightHand(const FVector &InOffset, bool bInValid)
    {
        LeftHandIKOffsetRightHand = InOffset;
        bHasValidLeftHandIKOffset = bInValid;
    }

    /** 实例唯一标识 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    /** 装备静态配置 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    /** 装备运行数据 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentRuntimeData> RuntimeData = nullptr;

    /** 单件装备行为系统 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentSystem> EquipmentSystem = nullptr;

    /** 当前装备表现实体 */
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABBBEquipmentPresentationActor> PresentationActor = nullptr;

    /** 装备初始化时缓存的左手 IK 相对右手骨骼位置 */
    FVector LeftHandIKOffsetRightHand = FVector::ZeroVector;

    /** 装备初始化时缓存的左手 IK 位置是否有效 */
    bool bHasValidLeftHandIKOffset = false;
};
