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

    /** @return 左手 IK 插槽在装备组件空间中的附加偏移 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    FVector GetLeftHandIKSocketOffset() const
    {
        return LeftHandIKSocketOffset;
    }

    /**
     * 查询装备插槽的当前组件空间变换与装备世界变换
     * @param SocketName                    装备插槽名称
     * @param OutSocketComponentSpace       接收插槽组件空间变换
     * @param OutEquipmentWorld             接收装备组件世界变换
     * @return 插槽与装备组件是否有效
     */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    bool TryGetSocketTransforms(
        FName SocketName,
        FTransform &OutSocketComponentSpace,
        FTransform &OutEquipmentWorld) const;

private:
    friend class UBBBEquipmentSystem;

    void SetLeftHandIKSocketOffset(const FVector &InSocketOffset)
    {
        LeftHandIKSocketOffset = InSocketOffset;
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

    /** 装备初始化时使用的左手 IK 插槽附加偏移 */
    FVector LeftHandIKSocketOffset = FVector::ZeroVector;
};
