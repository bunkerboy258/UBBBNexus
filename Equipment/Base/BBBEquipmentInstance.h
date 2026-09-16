#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"
#include "BBBEquipmentInstance.generated.h"

class ABBBEquipmentPresentationActor;
class UBBBEquipmentDefinition;
class UBBBEquipmentRuntimeData;
class UBBBAnimInstance;
class UBBBEquipmentAnimInstance;
class USkeletalMeshComponent;
class FBBBEquipmentAnimationSystem;

/** 单件装备的配置、运行状态与动作入口 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipmentInstance final : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 创建完整装备实例
     * @param Outer	实例生命周期所有者
     * @param InDefinition	装备静态配置
     * @return 创建完成的装备实例
     */
    static UBBBEquipmentInstance *Create(UObject &Outer, UBBBEquipmentDefinition &InDefinition);

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const;

    /** @return 当前装备表现实体 */
    UFUNCTION(BlueprintPure, Category = "BBB|Equipment", meta = (BlueprintThreadSafe))
    ABBBEquipmentPresentationActor *GetPresentationActor() const
    {
        return PresentationActor;
    }

    /**
     * 固定装备所属角色与挂接插槽
     * @param InCharacterMesh	角色骨骼网格
     * @param InAttachmentSocketName	右手挂接插槽
     * @return 绑定是否有效
     */
    bool BindHolder(USkeletalMeshComponent &InCharacterMesh, FName InAttachmentSocketName);

    /** @return 表现实体和动画链接是否成功激活 */
    bool Activate();

    /** 解除动画链接并释放表现实体 */
    void Deactivate();

    /** 清理装备与持有者的全部运行时关系 */
    void Shutdown();

    /**
     * 提交本地开火命令
     * @param Sequence	角色分配的动作序号
     * @return 是否成功开火
     */
    bool SubmitFire(int32 Sequence);

    /**
     * 提交本地换弹命令
     * @param WorldTimeSeconds	当前世界时间
     * @param Sequence	角色分配的动作序号
     * @return 是否开始换弹
     */
    bool SubmitReload(float WorldTimeSeconds, int32 Sequence);

    /**
     * 还原远端确认的装备动作
     * @param Event	网络还原的动作事件
     * @param WorldTimeSeconds	本地世界时间
     * @return 动作是否应用
     */
    bool ApplyRestoredAction(const FBBBEquipmentActionEvent &Event, float WorldTimeSeconds);

    /**
     * 推进单件装备的持续动作
     * @param WorldTimeSeconds	当前世界时间
     * @return 无
     */
    void AdvanceAction(float WorldTimeSeconds);

    /**
     * 一次性转交本帧装备动作结果
     * @param OutEvents	角色本帧装备事件容器
     * @return 无
     */
    void ConsumeEvents(FBBBCharacterEquipmentEvents &OutEvents);

    /** @return 当前是否正在换弹 */
    bool IsReloading() const;

    /** @return 装备动作持续时间 */
    float GetEquipDuration() const;

    /** @return 换弹动作持续时间 */
    float GetReloadDuration() const;

    /**
     * 构造装备动作的人物表现数据
     * @param OutPresentation	接收表现数据
     * @return 无
     */
    void BuildEquipActionPresentation(FBBBEquipmentActionPresentation &OutPresentation) const;

private:
    friend class FBBBEquipmentAnimationSystem;

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
    UPROPERTY(BlueprintReadOnly, Transient, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABBBEquipmentPresentationActor> PresentationActor = nullptr;

    UPROPERTY(Transient)
    TWeakObjectPtr<USkeletalMeshComponent> HolderMesh;

    FName AttachmentSocketName = NAME_None;

    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBAnimInstance> BoundCharacterAnimInstance;

    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBEquipmentAnimInstance> BoundWeaponAnimInstance;

    TArray<FBBBEquipmentActionEvent> PendingActions;
    TArray<FBBBEquipmentRecoilEvent> PendingRecoils;
};
