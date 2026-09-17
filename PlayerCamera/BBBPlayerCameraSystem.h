#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraConfig.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBPlayerCameraSystem.generated.h"
class ABBBCharacter;
class APlayerController;
class UCameraComponent;
class USpringArmComponent;

/** 独立玩家相机只持有角色弱引用 */
UCLASS()
class ABBB_EVAC_API ABBBPlayerCameraSystem final : public AActor
{
    GENERATED_BODY()

public:
    ABBBPlayerCameraSystem();
    virtual void Tick(float DeltaSeconds) override;
    /**
     * 绑定观察目标
     * @param InCharacter	观察角色
     * @param InController	本地控制器
     * @param InConfig	相机配置
     * @return 无
     */
    void Initialize(ABBBCharacter &InCharacter, APlayerController &InController,
        const FBBBPlayerCameraConfig &InConfig);
    /**
     * 提交相机贡献
     * @param Packet	相机输入
     * @return 无
     */
    void Submit(const FBBBPlayerCameraInput &Packet);

private:
    UPROPERTY()
    TObjectPtr<USpringArmComponent> Boom;
    UPROPERTY()
    TObjectPtr<UCameraComponent> Camera;
    TWeakObjectPtr<ABBBCharacter> Character;
    TWeakObjectPtr<APlayerController> Controller;
    FBBBPlayerCameraConfig Config;
    TArray<FBBBPlayerCameraInput> Pending;
    FVector2D RecoilOffset = FVector2D::ZeroVector;
    float RecoverySpeed = 1.0f;
};
