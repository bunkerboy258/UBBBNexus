#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputConfig.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/States/BBBCharacterInputStates.h"
#include "BBBPlayerInputSystem.generated.h"
class ABBBCharacter;
class ABBBPlayerCameraSystem;
class UEnhancedInputComponent;

/** 玩家输入适配层只向角色提交约定的数据包 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBPlayerInputSystem final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBPlayerInputSystem();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    /**
     * 绑定增强输入
     * @param Input	控制器输入组件
     * @return 无
     */
    void Bind(UEnhancedInputComponent &Input);
    /**
     * 切换输入目标并解除旧目标持续状态
     * @param Target	新角色或空
     * @return 无
     */
    void SetCharacter(ABBBCharacter *Target);
    /**
     * 设置外部输入屏蔽
     * @param bEnabled	是否允许玩家输入
     * @return 无
     */
    void SetInputEnabled(bool bEnabled);

    UPROPERTY(EditDefaultsOnly, Category = "BBB|Input")
    FBBBPlayerInputConfig Config;
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Camera")
    FBBBPlayerCameraConfig CameraConfig;
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Aim")
    float AimTargetDistance = 10000.0f;

private:
    void SubmitEquipSlot(int32 Slot);
    void SubmitReload();
    TWeakObjectPtr<ABBBCharacter> Character;
    UPROPERTY()
    TObjectPtr<ABBBPlayerCameraSystem> Camera;
    FBBBCharacterInputStates State;
    FVector2D MoveAxis = FVector2D::ZeroVector;
    FVector2D LookAxis = FVector2D::ZeroVector;
    bool bInputEnabled = true;
    bool bFire = false;
    bool bJump = false;
};
