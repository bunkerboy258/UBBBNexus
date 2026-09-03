#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipmentSystem.generated.h"

struct FBBBEquipmentFireResult;
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
    bool Equip(
        USkeletalMeshComponent &CharacterMesh,
        FName AttachmentSocketName);

    /**
     * 响应本地开火命令
     * @param CharacterAPI  角色能力接口
     * @return 是否成功开火
     */
    bool Fire(FBBBEquipmentFireResult &OutResult);

    /** @return 当前装备是否允许开始换弹 */
    bool CanStartReload() const;

    /** 完成换弹弹药结算 */
    void CommitReload();

    /**
     * 响应远端开火表现命令
     * @param CharacterAPI  角色能力接口
     */
    void PresentFire();

    /** 释放表现实体并保留运行数据 */
    void ReleasePresentation();

    /** @return 装备动作持续时间 */
    float GetEquipDuration() const;

    /** @return 换弹动作持续时间 */
    float GetReloadDuration() const;

    /** @return 弹匣卸下归一化时刻 */
    float GetMagazineRemoveNormalizedTime() const;

    /** @return 弹匣生成归一化时刻 */
    float GetMagazineSpawnNormalizedTime() const;

    /**
     * 查询瞄准来源相对右手骨骼的固定变换
     * @param OutTransform	接收右手骨骼空间变换
     * @return 固定变换有效时返回true
     */
    bool TryGetAimSourceRightHandBoneSpace(FTransform &OutTransform) const;

    /**
     * 查询距离上次开火的时间
     * @param WorldTimeSeconds 当前世界时间
     * @return 未曾开火时返回一个足够大的安全值
     */
    float GetTimeSinceLastFire(float WorldTimeSeconds) const;

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
