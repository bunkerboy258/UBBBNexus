#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"
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

    /** @return 玩家玩法输入是否启用 */
    bool IsInputEnabled() const
    {
        return bInputEnabled;
    }

    UPROPERTY(EditDefaultsOnly, Category = "BBB|Input")
    FBBBPlayerInputConfig Config;
    /** 本地玩家使用的相机蓝图类 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Camera")
    TSubclassOf<ABBBPlayerCameraSystem> CameraClass;
    /** 视角输入每单位对应的旋转角度 */
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Input", meta = (ClampMin = "0.0"))
    float BaseTurnRate = 1.0f;
    UPROPERTY(EditDefaultsOnly, Category = "BBB|Aim")
    float AimTargetDistance = 10000.0f;

private:
    void SubmitEquipSlot(int32 Slot);
    void SubmitReload();
    TWeakObjectPtr<ABBBCharacter> Character;
    UPROPERTY()
    TObjectPtr<ABBBPlayerCameraSystem> Camera;
    FBBBCharacterMovementPacket MovementState;
    FBBBCharacterAimPacket AimState;
    FVector2D MoveAxis = FVector2D::ZeroVector;
    FVector2D LookAxis = FVector2D::ZeroVector;
    bool bInputEnabled = true;
    bool bFire = false;
    bool bJump = false;
};
