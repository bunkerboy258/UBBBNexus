#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentSystem.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UBBBEquipmentDefinition;
class UBBBEquipmentInstance;
class UBBBEquipmentRuntimeData;
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

    /** @return 当前装备表现实体 */
    ABBBEquipmentPresentationActor *GetPresentationActor() const;

    /** @return 当前是否处于装备过渡 */
    bool IsEquipping() const;

private:
    friend class UBBBEquipmentInstance;

    /**
     * 注入单件装备永久依赖
     * @param InDefinition  装备静态配置
     * @param InRuntimeData 装备运行数据
     * @return 依赖是否完整有效
     */
    bool Initialize(
        UBBBEquipmentDefinition &InDefinition,
        UBBBEquipmentRuntimeData &InRuntimeData);

    /** 装备静态配置 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    /** 装备运行数据 */
    UPROPERTY()
    TObjectPtr<UBBBEquipmentRuntimeData> RuntimeData = nullptr;

    /** 当前装备表现实体 */
    UPROPERTY()
    TObjectPtr<ABBBEquipmentPresentationActor> PresentationActor = nullptr;
};
