#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/ExternalAPI/BBBEquipmentExternalAPI.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterEquipmentEvent.h"
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
class FBBBCharacterShutdown;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentActionProcessor;
class FBBBCharacterAnimationLayerProcessor;
class FBBBEquipmentUploadProcessor;
class FBBBCharacterExternalAPI;

/** 单件装备的实体、运行数据与公开操作入口 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBEquipmentInstance final : public AActor
{
    GENERATED_BODY()

public:
    ABBBEquipmentInstance();

    /** @return 装备命令入口 */
    FBBBEquipmentExternalAPI &GetExternalAPI()
    {
        return ExternalAPI;
    }

protected:
    virtual void BeginPlay() override;

private:
    friend class FBBBCharacterShutdown;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBEquipmentUploadProcessor;
    friend class FBBBCharacterExternalAPI;

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
     * 在角色骨骼更新完成后消费命令并发布快照
     * @param DeltaSeconds	帧间隔
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;

    friend class FBBBEquipmentExternalAPI;
    friend class FBBBEquipmentCommandExecutor;
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
    FBBBEquipmentExternalAPI ExternalAPI;
    TArray<FBBBEquipmentCommand> PendingCommands;
    TArray<FBBBEquipmentActionEvent> PendingSnapshots;

    FName AttachmentSocketName = NAME_None;
    bool bIsMirror = false;
    bool bIsActive = false;
    FBBBEquipmentEquipSystem EquipSystem;
    FBBBEquipmentFireSystem FireSystem;
    FBBBEquipmentReloadSystem ReloadSystem;
    FBBBEquipmentAnimationSystem AnimationSystem;
};
