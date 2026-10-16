#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBZombieHitTrajectoryComponent.generated.h"

class ABBBZombieHitReactionTarget;
class USkeletalMeshComponent;
class UPhysicsAsset;
class UHitReactProfile;
class FJsonObject;

/** 调试靶专用的末帧骨骼与刚体采样 不参与正式小怪逻辑 */
UCLASS()
class ABBB_EVAC_API UBBBZombieHitTrajectoryComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    /** @return 默认关闭且在物理结束后采样的组件 */
    UBBBZombieHitTrajectoryComponent();
    /** @param RequestJson	有界试验配置 @return 启动结果 */
    FString Start(const FString& RequestJson);
    /** @return 当前试验状态与报告路径 */
    FString Status() const;
    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End UActorComponent Interface
private:
    /** 同相位且无物理受击的参考骨架 */
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> ReferenceMesh;
    /** 参考骨架使用独立拥有者 不影响正式组件查找目标网格 */
    UPROPERTY(Transient)
    TObjectPtr<AActor> ReferenceActor;
    /** 每轮独立物理资产 防止修改正式资产 */
    UPROPERTY(Transient)
    TObjectPtr<UPhysicsAsset> ProbePhysics;
    /** 首次采样时的原始物理资产 每轮从此重新复制 */
    UPROPERTY(Transient)
    TObjectPtr<UPhysicsAsset> SourcePhysics;
    /** 每轮独立配置 防止修改正式资产 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UHitReactProfile>> ProbeProfiles;
    /** 首次采样时的原始部位配置 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UHitReactProfile>> SourceProfiles;
    /** 当前诊断结果 不保留已结束试验的运行状态 */
    TSharedPtr<FJsonObject> Report;
    TSharedPtr<FJsonObject> Request;
    double StartSeconds = 0.0;
    int32 Fired = 0;
    FString OutputPath;
    /** @param Error	空字符串表示正常结束 @return 无返回值 */
    void Complete(const FString& Error = FString());
};
