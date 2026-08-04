#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentInstance.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;
class UBBBEquipmentRuntimeData;
class USkeletalMeshComponent;

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

    /**
     * 生成并挂接装备表现实体
     * @param CharacterMesh		角色骨骼网格
     * @param CharacterAPI		角色能力接口
     * @param AttachmentSocketName	装备挂接插槽
     */
    void Equip(
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName);

    /**
     * 更新装备内部领域
     * @param CharacterAPI	角色能力接口
     */
    void Update(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应本地开火命令
     * @param CharacterAPI	角色能力接口
     * @return 是否成功开火
     */
    bool Fire(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应本地换弹命令
     * @param CharacterAPI	角色能力接口
     * @return 是否成功开始换弹
     */
    bool Reload(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应远端开火表现命令
     * @param CharacterAPI	角色能力接口
     */
    void PresentFire(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应远端换弹表现命令
     * @param CharacterAPI	角色能力接口
     */
    void PresentReload(FBBBCharacterExternalAPI &CharacterAPI);

    /** 释放表现实体并保留运行数据 */
    void ReleasePresentation();

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const;

    /** @return 当前装备表现实体 */
    ABBBEquipmentPresentationActor *GetPresentationActor() const;

    /** @return 当前是否处于装备过渡 */
    bool IsEquipping() const;

    /** @return 实例是否完整有效 */
    bool IsValid() const;

private:
    /** 实例唯一标识 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    /** 装备静态配置 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    /** 装备运行数据 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentRuntimeData> RuntimeData = nullptr;

    /** 当前装备表现实体 */
    UPROPERTY()
    TObjectPtr<ABBBEquipmentPresentationActor> PresentationActor = nullptr;
};
