
#pragma once
#include "CoreMinimal.h"
class APawn;
class UCharacterMovementComponent;
class FBBBCharacterInitializer;
struct FBBBAimRuntimeData;
struct FBBBCharacterFacingConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

class ABBB_EVAC_API FBBBCharacterFacingSystem final
{
public:

    /**
     * 每帧更新角色朝向 瞄准时按瞄准角触发身体转向插值
     */
    void Update();
private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化朝向系统依赖与配置
     * @param InPawn	所属角色Pawn
     * @param InMovement	角色移动组件
     * @param InFacingData	朝向运行时数据
     * @param InWorldData	世界运行时数据
     * @param InIntentData	意图运行时数据
     * @param InAimData	瞄准运行时数据
     * @param InConfig	朝向配置
     */
    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        FBBBFacingRuntimeData &InFacingData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBAimRuntimeData &InAimData,
        const FBBBCharacterFacingConfig &InConfig);

    APawn *Pawn = nullptr;

    UCharacterMovementComponent *Movement = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    FBBBFacingRuntimeData *FacingData = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBAimRuntimeData *AimData = nullptr;

    const FBBBCharacterFacingConfig *Config = nullptr;
};
