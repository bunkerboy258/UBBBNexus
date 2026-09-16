#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipmentActionResult.h"
#include "BBBWork/UBBBNexus/Equipment/Runtime/BBBEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/System/AnimationSystem/BBBEquipmentAnimationSystem.h"
#include "BBBWork/UBBBNexus/Equipment/System/EquipSystem/BBBEquipmentEquipSystem.h"
#include "BBBWork/UBBBNexus/Equipment/System/FireSystem/BBBEquipmentFireSystem.h"
#include "BBBWork/UBBBNexus/Equipment/System/ReloadSystem/BBBEquipmentReloadSystem.h"
#include "GameFramework/Actor.h"
#include "BBBEquipmentInstance.generated.h"

class ABBBCharacterInstance;
class FBBBCharacterExternalAPI;
class UBBBEquipmentAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;
class FBBBEquipmentInitializer;
class FBBBEquipmentUpdatePipeline;

/** 单件装备的实体、运行数据与公开操作入口 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBEquipmentInstance final : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipmentInstance();

    /**
     * 创建引用配置的装备实体
     * @param Holder            持有角色
     * @param InDefinition      装备静态配置
     * @param bInIsMirror       是否为角色决定的镜像实例
     * @return 创建完成的装备实体
     */
    static ABBBEquipmentInstance *Create(
        ABBBCharacterInstance &Holder,
        UBBBEquipmentDefinition &InDefinition,
        bool bInIsMirror);

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

    /** @return 装备骨骼网格 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment")
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /**
     * 固定角色网格和挂接插槽
     * @param InCharacterMesh       角色骨骼网格
     * @param InAttachmentSocketName 右手挂接插槽
     * @return 绑定是否有效
     */
    bool BindHolder(USkeletalMeshComponent &InCharacterMesh, FName InAttachmentSocketName);

    /** @return 装备和动画链接是否成功激活 */
    bool Activate();

    /** 解除动画链接并隐藏装备 */
    void Deactivate();

    /** 清理装备持有关系并销毁实体 */
    void Shutdown();

    /**
     * 发布一次装备人物表现
     * @param DurationOverride  角色给出的持续时间，非正值使用配置
     * @param OutResult         装备决定的人物表现
     * @return 是否发布成功
     */
    bool BeginEquipAction(float DurationOverride, FBBBEquipmentActionResult &OutResult);

    /**
     * 执行一次开火
     * @param OutResult         装备决定的人物表现与本地后坐力
     * @return 是否成功执行
     */
    bool SubmitFire(FBBBEquipmentActionResult &OutResult);

    /**
     * 开始一次换弹
     * @param WorldTimeSeconds  当前世界时间
     * @param Sequence          角色分配的操作序号
     * @param DurationOverride  角色给出的持续时间，非正值使用配置
     * @param OutResult         装备决定的人物表现
     * @return 是否成功开始
     */
    bool SubmitReload(
        float WorldTimeSeconds,
        int32 Sequence,
        float DurationOverride,
        FBBBEquipmentActionResult &OutResult);

    /** 推进持续操作状态 */
    void AdvanceAction(float WorldTimeSeconds);

    /** 在角色移动后一次性发布动画事实 */
    void PublishAnimationFacts(float WorldTimeSeconds);

    /** @return 当前是否正在换弹 */
    bool IsReloading() const;

protected:
    virtual void BeginPlay() override;

private:
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;
    friend class FBBBEquipmentEquipSystem;
    friend class FBBBEquipmentFireSystem;
    friend class FBBBEquipmentReloadSystem;
    friend class FBBBEquipmentAnimationSystem;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    TWeakObjectPtr<USkeletalMeshComponent> HolderMesh;

    UPROPERTY(Transient)
    FBBBEquipmentRuntimeData RuntimeData;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;

    FName AttachmentSocketName = NAME_None;
    bool bIsMirror = false;
    bool bIsActive = false;
    FBBBEquipmentEquipSystem EquipSystem;
    FBBBEquipmentFireSystem FireSystem;
    FBBBEquipmentReloadSystem ReloadSystem;
    FBBBEquipmentAnimationSystem AnimationSystem;
};
