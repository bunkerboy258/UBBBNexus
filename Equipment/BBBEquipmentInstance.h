#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/ExternalAPI/BBBEquipmentExternalAPI.h"
#include "BBBWork/UBBBNexus/Equipment/Pipeline/Input/BBBEquipmentInputPipeline.h"
#include "BBBWork/UBBBNexus/Equipment/Core/Update/BBBEquipmentUpdatePipeline.h"
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
class FBBBCharacterEquipmentLifecycleProcessor;
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

private:
    friend class FBBBCharacterShutdown;
    friend class FBBBCharacterEquipmentLifecycleProcessor;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterAnimationLayerProcessor;
    friend class FBBBEquipmentUploadProcessor;
    friend class FBBBCharacterExternalAPI;

    /** @return 实例唯一标识 */
    const FGuid &GetInstanceId() const;

    /** @return 装备配置标识 */
    FName GetEquipmentId() const;

    /** @return 角色应链接的动画层类型 */
    TSubclassOf<UAnimInstance> GetCharacterAnimationLayerClass() const;

    /** @return 装备骨骼网格 */
    USkeletalMeshComponent *GetEquipmentSkeletalMesh() const;

    /**
     * 在角色骨骼更新完成后消费命令并发布快照
     * @param DeltaSeconds	帧间隔
     * @return 无
     */
    virtual void Tick(float DeltaSeconds) override;

    friend class FBBBEquipmentExternalAPI;
    friend class FBBBEquipmentInitializer;
    friend class FBBBEquipmentUpdatePipeline;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FGuid InstanceId;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UBBBEquipmentDefinition> Definition = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArrowComponent> EquipmentRoot = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Equipment", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> EquipmentSkeletalMesh = nullptr;

    UPROPERTY(Transient)
    FBBBEquipmentRuntimeData RuntimeData;

    FBBBEquipmentExternalAPI ExternalAPI;
    FBBBEquipmentInputPipeline InputPipeline;
    FBBBEquipmentUpdatePipeline UpdatePipeline;
    bool bIsMirror = false;
    FBBBEquipmentEquipSystem EquipSystem;
    FBBBEquipmentFireSystem FireSystem;
    FBBBEquipmentReloadSystem ReloadSystem;
    FBBBEquipmentAnimationSystem AnimationSystem;
};
