
#pragma once
#include "CoreMinimal.h"
class APawn;
class UCharacterMovementComponent;
class FBBBCharacterInitializer;
struct FBBBCharacterLocomotionConfig;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterLocomotionSystem final
{
public:

    /**
     * 逐帧根据移动意图驱动角色移动 更新移动参数并施加移动输入
     */
    void Update();
private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化移动系统依赖
     * @param InPawn	受控Pawn
     * @param InMovement	角色移动组件
     * @param InIntentData	移动意图运行时数据
     * @param InConfig	移动配置
     */
    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterLocomotionConfig &InConfig);

    APawn *Pawn = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBCharacterLocomotionConfig *Config = nullptr;
};
