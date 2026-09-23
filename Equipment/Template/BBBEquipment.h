#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBEquipment.generated.h"

struct FBBBEquipmentActionFact;
class UBBBEquipmentAnimInstance;
class UBBBEquipmentDefinition;
class UArrowComponent;
class USkeletalMeshComponent;
class FBBBCharacterShutdown;
class FBBBCharacterEquipmentLifecycleProcessor;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentActionProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBEquipmentStateObservationProcessor;

/** 单件装备的共享演员、配置和固定行为入口 */
UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API ABBBEquipment : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipment();

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 装备静态配置 */
    UBBBEquipmentDefinition *GetDefinition() const
    {
        return Definition;
    }

    /** @return 是否为远端镜像实例 */
    bool IsMirror() const
    {
        return bIsMirror;
    }

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /** @return 装备动画实例 */
    UBBBEquipmentAnimInstance *GetEquipmentAnimationInstance() const;

    /**
     * 提交装备动作输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitEquipInput(int32 Sequence, bool bInIsMirror);

    /**
     * 提交主行为输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitPrimaryInput(int32 Sequence, bool bInIsMirror);

    /**
     * 提交副行为输入
     * @param bActive        副行为是否激活
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitSecondaryInput(bool bActive, int32 Sequence, bool bInIsMirror);

    /**
     * 提交换弹输入
     * @param Sequence       动作序号
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitReloadInput(int32 Sequence, bool bInIsMirror);

    /**
     * 提交已经成立的镜像事实
     * @param Fact           已确认装备事实
     * @param bInIsMirror    是否只能执行事实还原
     * @return 无
     */
    virtual void SubmitRestoreFact(const FBBBEquipmentActionFact &Fact, bool bInIsMirror);

private:
    friend class FBBBCharacterShutdown;
    friend class FBBBCharacterEquipmentLifecycleProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBEquipmentStateObservationProcessor;

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

protected:
    /**
     * 初始化装备公共演员数据
     * @param InDefinition    装备静态配置
     * @param InInstanceId    实例唯一标识
     * @param bInIsMirror     是否为远端镜像
     * @return 初始化是否成功
     */
    bool InitializeEquipment(const FGuid &InInstanceId, bool bInIsMirror);

    /**
     * 检查输入声明的镜像身份是否与装备实例一致
     * @param bInIsMirror    调用者声明的镜像身份
     * @return 是否允许接收输入
     */
    bool ValidateInputSource(bool bInIsMirror) const;

    /**
     * 根据具体装备配置初始化运行时数据
     * @param InDefinition    装备静态配置
     * @return 初始化是否成功
     */
    virtual bool InitializeRuntimeData();

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    bool bIsMirror = false;

    UPROPERTY(Transient)
    TObjectPtr<UBBBEquipmentAnimInstance> EquipmentAnimationInstance = nullptr;
};
