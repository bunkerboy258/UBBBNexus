#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentSystem.generated.h"

class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;
class UBBBEquipmentInstance;
class UBBBEquipmentRuntimeData;
class FBBBCharacterEquipmentActionProcessor;
class USkeletalMeshComponent;

/** 单件装备的领域编排与表现生命周期系统 */
UCLASS()
class ABBB_EVAC_API UBBBEquipmentSystem final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 生成并挂接装备表现实体
     * @param CharacterMesh         角色骨骼网格
     * @param CharacterAPI          角色能力接口
     * @param AttachmentSocketName  装备挂接插槽
     */
    void Equip(
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName);

    /**
     * 更新装备内部领域
     * @param CharacterAPI  角色能力接口
     */
    void Update(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应本地开火命令
     * @param CharacterAPI  角色能力接口
     * @return 是否成功开火
     */
    bool Fire(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应本地换弹命令
     * @param CharacterAPI  角色能力接口
     * @return 是否成功开始换弹
     */
    bool Reload(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应远端开火表现命令
     * @param CharacterAPI  角色能力接口
     */
    void PresentFire(FBBBCharacterExternalAPI &CharacterAPI);

    /**
     * 响应远端换弹表现命令
     * @param CharacterAPI  角色能力接口
     */
    void PresentReload(FBBBCharacterExternalAPI &CharacterAPI);

    /** 释放表现实体并保留运行数据 */
    void ReleasePresentation();

    /** @return 当前是否处于装备过渡 */
    bool IsEquipping() const;

    /**
     * 查询瞄准来源相对右手骨骼的固定变换
     * @param OutTransform	接收右手骨骼空间变换
     * @return 固定变换有效时返回true
     */
    bool TryGetAimSourceRightHandBoneSpace(FTransform &OutTransform) const;

    /**
     * 查询左手目标相对右手骨骼的固定变换
     * @param OutTransform	接收右手骨骼空间变换
     * @return 固定变换有效时返回true
     */
    bool TryGetLeftHandTargetRightHandBoneSpace(FTransform &OutTransform) const;

    /** @return 是否启用左手逆向动力学 */
    bool IsLeftHandIKEnabled() const;

private:
    friend class UBBBEquipmentInstance;
    friend class FBBBCharacterEquipmentActionProcessor;

    /** 拔出并掉落当前弹匣表现 */
    void RemoveMagazine();

    /** 生成并装入新弹匣表现 */
    void SpawnMagazine();

    /**
     * 注入单件装备永久依赖
     * @param InDefinition  装备静态配置
     * @param InRuntimeData 装备运行数据
     * @return 依赖是否完整有效
     */
    bool Initialize(
        UBBBEquipmentInstance &InInstance,
        UBBBEquipmentDefinition &InDefinition,
        UBBBEquipmentRuntimeData &InRuntimeData);

    /** 所属装备实例 */
    UBBBEquipmentInstance *Instance = nullptr;

    /** 装备静态配置 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    /** 装备运行数据 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentRuntimeData> RuntimeData = nullptr;
};
