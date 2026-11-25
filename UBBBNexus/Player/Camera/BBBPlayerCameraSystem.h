#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Player/Camera/Input/BBBPlayerCameraInput.h"
#include "BBBWork/UBBBNexus/Player/Camera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBWork/UBBBNexus/Character/Config/Camera/BBBCharacterCameraConfig.h"
#include "BBBPlayerCameraSystem.generated.h"
class ABBBCharacter;
class APlayerController;
class UCameraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USpringArmComponent;
class FBBBPlayerCameraImpulseProcessor;
class UBBBAnimInstance;
class UAnimInstance;

/** 独立玩家相机只持有角色弱引用 构图与回正读取角色 Camera 配置 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBPlayerCameraSystem : public AActor
{
    GENERATED_BODY()

public:
    ABBBPlayerCameraSystem();
    virtual void Tick(float DeltaSeconds) override;
    /** @param EndPlayReason 结束原因 @return 无 释放相机专属轮廓材质 */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    /**
     * 绑定观察目标
     * @param InCharacter	观察角色
     * @param InController	本地控制器
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, APlayerController &InController);
    /**
     * 提交相机贡献
     * @param Packet	相机输入
     * @return 无
     */
    void Submit(const FBBBPlayerCameraInput &Packet);

    /**
     * 从蓝图指定的动画快照读取本地镜头贡献
     * @param CharacterAnimation	被观察角色的动画实例
     * @param Source			贡献来源 用于隔离不同装备的序号
     * @param FireSequence		已经成立的开火序号
     * @param Settings			当前姿态下的相机配置
     * @return 是否存在有效贡献来源
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "BBB|相机|冲量")
    bool ReadRecoilSource(UBBBAnimInstance *CharacterAnimation, UAnimInstance *&Source,
        int32 &FireSequence, FBBBPlayerCameraRecoilSettings &Settings);

private:
    friend class FBBBPlayerCameraImpulseProcessor;

    /** 根据瞄准和站蹲进度应用构图与本机景深 */
    void ApplyComposition(float AimAlpha, float CrouchAlpha);

    /** 仅本地玩家视图使用的可见内侧轮廓材质 */
    UPROPERTY(EditDefaultsOnly, Category = "相机|角色可读性", meta = (DisplayName = "角色内侧轮廓材质"))
    TSoftObjectPtr<UMaterialInterface> InnerOutlineMaterial;
    /** 相机独占的轮廓参数 不写入共享材质 */
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> InnerOutlineInstance;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|相机", meta = (AllowPrivateAccess = "true", DisplayName = "相机臂"))
    TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|相机", meta = (AllowPrivateAccess = "true", DisplayName = "相机"))
    TObjectPtr<UCameraComponent> Camera;
    /** 绑定角色后缓存的构图与回正 */
    FBBBCharacterCameraConfig CameraConfig;
    /** 瞄准过渡的线性进度 取值从零到一 */
    float AimBlendProgress = 0.0f;
    /** 当前平滑后的胶囊参考高度 */
    float SmoothedCapsuleHalfHeight = 0.0f;
    /** 本次高度过渡起点 */
    float CapsuleHeightBlendStart = 0.0f;
    /** 实际胶囊提供的高度目标 */
    float CapsuleHeightBlendTarget = 0.0f;
    /** 当前高度过渡已经经过的时间 */
    float CapsuleHeightBlendElapsed = 0.0f;
    TWeakObjectPtr<ABBBCharacter> Character;
    TWeakObjectPtr<APlayerController> Controller;
    /** 当前等待消费的累计相机冲击 */
    TOptional<FBBBPlayerCameraInput> Pending;

    /** 当前相机三轴冲击偏移 */
    FVector RecoilOffset = FVector::ZeroVector;

    /** 未读取到武器贡献时采用的相机冲击参数 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|冲量", meta = (DisplayName = "默认相机冲击", ToolTip = "无武器贡献时使用的镜头冲击参数 回正速度始终取自角色相机配置"))
    FBBBPlayerCameraRecoilSettings DefaultRecoilSettings;

    /** 最近有效贡献使用的冲击参数 */
    UPROPERTY(Transient)
    FBBBPlayerCameraRecoilSettings ActiveRecoilSettings;

    /** 当前镜头贡献来源 */
    TWeakObjectPtr<UAnimInstance> RecoilSource;

    /** 已经消费的来源开火序号 */
    UPROPERTY(Transient)
    int32 LastFireSequence = 0;

    /** 已消费的角色相机贡献序号 */
    UPROPERTY(Transient)
    uint64 LastCameraRevision = 0;
};
