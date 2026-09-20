#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Definition/BBBEquipmentCommand.h"
#include "GameFramework/Actor.h"
#include "BBBEquipment.generated.h"

class ABBBCharacter;
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

/** 单件装备的实体、运行数据与公开操作入口 */
UCLASS(BlueprintType)
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

    /**
     * 提交角色跨入装备领域的统一命令
     * @param Command    待映射的通用命令
     * @return 无
     */
    virtual void SubmitCommand(const FBBBEquipmentCommand &Command);

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

    /**
     * 在角色骨骼更新完成后消费命令并发布快照
     * @param DeltaSeconds	帧间隔
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;

protected:
    /**
     * 初始化装备公共演员数据
     * @param InDefinition    装备静态配置
     * @param InInstanceId    实例唯一标识
     * @param bInIsMirror     是否为远端镜像
     * @return 初始化是否成功
     */
    bool InitializeEquipment(UBBBEquipmentDefinition &InDefinition, const FGuid &InInstanceId, bool bInIsMirror);

    /**
     * 在武器骨骼动画完成后解析当前帧输入
     * @param DeltaSeconds    帧间隔
     * @return 无
     */
    virtual void UpdateEquipment(float DeltaSeconds);

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    bool bIsMirror = false;
};
