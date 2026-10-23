#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Player/Camera/Input/BBBPlayerCameraInput.h"
#include "BBBWork/UBBBNexus/Player/Camera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBPlayerCameraSystem.generated.h"
class ABBBCharacter;
class APlayerController;
class UCameraComponent;
class USpringArmComponent;
class FBBBPlayerCameraImpulseProcessor;
class UBBBAnimInstance;
class UAnimInstance;

/** 独立玩家相机只持有角色弱引用 */
UCLASS(Blueprintable)
class ABBB_EVAC_API ABBBPlayerCameraSystem : public AActor
{
    GENERATED_BODY()

public:
    ABBBPlayerCameraSystem();
    virtual void Tick(float DeltaSeconds) override;
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|相机", meta = (AllowPrivateAccess = "true", DisplayName = "相机臂"))
    TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|相机", meta = (AllowPrivateAccess = "true", DisplayName = "相机"))
    TObjectPtr<UCameraComponent> Camera;
    /** 常态相机臂长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "1.0", DisplayName = "常态相机臂长度"))
    float HipArmLength = 410.0f;
    /** 瞄准相机臂长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "1.0", DisplayName = "瞄准相机臂长度"))
    float AimArmLength = 225.0f;
    /** 常态镜头水平视角 单位度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "30.0", ClampMax = "120.0", DisplayName = "常态水平视角"))
    float HipFieldOfView = 80.0f;
    /** 瞄准镜头水平视角 单位度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "30.0", ClampMax = "120.0", DisplayName = "瞄准水平视角"))
    float AimFieldOfView = 66.0f;
    /** 常态镜头向右偏移 单位厘米 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "0.0", DisplayName = "常态肩位偏移"))
    float HipShoulderOffset = 65.0f;
    /** 瞄准镜头向右偏移 单位厘米 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|构图", meta = (ClampMin = "0.0", DisplayName = "瞄准肩位偏移"))
    float AimShoulderOffset = 58.0f;
    /** 瞄准距离切换速度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机", meta = (ClampMin = "0.1", DisplayName = "瞄准相机臂插值速度"))
    float AimBoomInterpSpeed = 12.0f;
    TWeakObjectPtr<ABBBCharacter> Character;
    TWeakObjectPtr<APlayerController> Controller;
    /** 当前等待消费的累计相机冲击 */
    TOptional<FBBBPlayerCameraInput> Pending;

    /** 当前相机三轴冲击偏移 */
    FVector RecoilOffset = FVector::ZeroVector;

    /** 未读取到武器贡献时采用的相机基础参数 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|相机|冲量", meta = (DisplayName = "默认相机后坐力", ToolTip = "无外部配置时使用的镜头冲击与恢复设置"))
    FBBBPlayerCameraRecoilSettings DefaultRecoilSettings;

    /** 最近有效贡献使用的恢复参数 */
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
