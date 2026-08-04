#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBItemInstance.generated.h"

class ABBBEquipmentActor;
class FBBBCharacterExternalAPI;
class FBBBEquipmentRestoreProcessor;
class UBBBItemDefinition;
class UBBBItemRuntimeData;
class USkeletalMeshComponent;

/** 物品配置运行数据与装备实体的唯一实例根 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBItemInstance final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 创建完整物品实例
     * @param Outer             实例生命周期所有者
     * @param InDefinition      物品静态配置
     * @param InStackCount      堆叠数量
     * @return 创建完成的物品实例
     */
    static UBBBItemInstance *Create(UObject &Outer, UBBBItemDefinition &InDefinition, int32 InStackCount);

    /**
     * 生成并挂接装备实体
     * @param CharacterMesh         角色骨骼网格
     * @param CharacterAPI          角色能力接口
     * @param AttachmentSocketName  装备挂接插槽
     */
    void Equip(
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName);

    /** 释放装备实体并保留运行数据 */
    void ReleaseModel();

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 物品静态配置 */
    UBBBItemDefinition *GetDefinition() const;

    /** @return 物品运行数据 */
    UBBBItemRuntimeData *GetRuntimeData() const;

    /** @return 当前堆叠数量 */
    int32 GetStackCount() const;

    /** @return 当前生成的装备实体 */
    ABBBEquipmentActor *GetModelActor() const;

    /** @return 装备实体是否正在执行装备过渡 */
    bool IsEquipping() const;

    /** @return 是否配置可装备操作域 */
    bool IsEquipable() const;

    /** @return 实例标识与静态配置是否有效 */
    bool IsValid() const;

private:
    friend class FBBBEquipmentRestoreProcessor;

    /**
     * 创建仅用于远端表现的镜像实例
     * @param Outer             实例生命周期所有者
     * @param InDefinition      物品静态配置
     * @return 创建完成的镜像实例
     */
    static UBBBItemInstance *CreateMirror(UObject &Outer, UBBBItemDefinition &InDefinition);

    /** 实例唯一标识 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    /** 物品静态配置 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemDefinition> Definition = nullptr;

    /** 完整实例运行数据 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBItemRuntimeData> RuntimeData = nullptr;

    /** 当前生成的装备实体 */
    UPROPERTY()
    TObjectPtr<ABBBEquipmentActor> ModelActor = nullptr;

    /** 当前堆叠数量 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 StackCount = 1;
};
