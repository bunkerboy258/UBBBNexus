#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBPlayerCameraSystem.generated.h"
class ABBBCharacter;
class APlayerController;
class UCameraComponent;
class USpringArmComponent;

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

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BBB|Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> Camera;
    /** 瞄准时的相机臂长度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Camera", meta = (ClampMin = "0.0"))
    float AimBoomLength = 180.0f;
    /** 瞄准距离切换速度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Camera", meta = (ClampMin = "0.1"))
    float AimBoomInterpSpeed = 12.0f;
    /** 初始化时读取组件配置 仅用于退出瞄准后恢复常态距离 */
    float DefaultBoomLength = 0.0f;
    TWeakObjectPtr<ABBBCharacter> Character;
    TWeakObjectPtr<APlayerController> Controller;
    TArray<FBBBPlayerCameraInput> Pending;
    FVector2D RecoilOffset = FVector2D::ZeroVector;
    float RecoverySpeed = 1.0f;
};
