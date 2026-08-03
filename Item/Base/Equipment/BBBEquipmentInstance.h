#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBEquipmentInstance.generated.h"

class ABBBEquipmentActor;
class FBBBCharacterExternalAPI;
class FBBBEquipmentRestoreProcessor;
class UBBBEquipmentDefinition;
class USkeletalMeshComponent;

/** 装备配置运行数据与模型的唯一实例根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentInstance final : public UBBBItemInstance
{
    GENERATED_BODY()

public:

    /**
     * 创建完整装备实例
     * @param Outer\t实例生命周期所有者
     * @param InDefinition\t装备静态配置
     * @param InStackCount\t堆叠数量
     * @return 创建完成的装备实例
     */
    static UBBBEquipmentInstance *Create(UObject &Outer, UBBBEquipmentDefinition &InDefinition, int32 InStackCount);

    /**
     * 初始化持久运行数据并生成当前模型
     * @param CharacterMesh\t角色骨骼网格
     * @param CharacterAPI\t角色能力接口
     * @param AttachmentSocketName\t装备挂接插槽
     * @param WorldTimeSeconds\t当前世界时间
     */
    void Equip(
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName,
        float WorldTimeSeconds);

    /** 释放临时模型并保留持久运行数据 */
    void ReleaseModel();

    /** @param WorldTimeSeconds 当前世界时间 */
    void Update(float WorldTimeSeconds);

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetEquipmentDefinition() const;

    /** @return 当前生成的装备模型 */
    ABBBEquipmentActor *GetModelActor() const;

    /** @return 是否正在执行装备过渡 */
    bool IsEquipping() const;

private:

    friend class FBBBEquipmentRestoreProcessor;

    enum class EInstanceMode : uint8
    {
        Runtime,
        Mirror
    };

    /**
     * 创建表现镜像实例
     * @param Outer\t实例生命周期所有者
     * @param InDefinition\t装备静态配置
     * @return 创建完成的镜像实例
     */
    static UBBBEquipmentInstance *CreateMirror(UObject &Outer, UBBBEquipmentDefinition &InDefinition);

    /** 是否已完成一次持久运行数据初始化 */
    UPROPERTY()
    bool bIsRuntimeDataInitialized = false;

    /** 当前临时装备模型 */
    UPROPERTY()
    TObjectPtr<ABBBEquipmentActor> ModelActor = nullptr;

    /** 是否正在执行装备过渡 */
    UPROPERTY()
    bool bIsEquipping = false;

    /** 装备过渡结束时间 */
    UPROPERTY()
    float EquipEndTime = 0.0f;

    /** 仅供初始化阶段选择完整或表现装配 */
    EInstanceMode InstanceMode = EInstanceMode::Runtime;
};
