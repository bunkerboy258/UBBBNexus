#pragma once

#include "CoreMinimal.h"

class APawn;
class FBBBCharacterInitializer;
class UCharacterMovementComponent;
class USkeletalMeshComponent;
struct FBBBAimRuntimeData;
struct FBBBCharacterFacingConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBFacingRuntimeData;
struct FBBBIntentRuntimeData;

/** 统一控制角色移动朝向与瞄准身体追赶 */
class ABBB_EVAC_API FBBBCharacterFacingSystem final
{
public:
    /** 每帧更新角色朝向 */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化朝向系统依赖
     * @param InPawn		所属角色
     * @param InMovement		角色移动组件
     * @param InCharacterMesh	角色骨骼网格组件
     * @param InFacingData	朝向运行数据
     * @param InWorldData		世界运行数据
     * @param InIntentData		意图运行数据
     * @param InAimData		瞄准运行数据
     * @param InConfig		朝向配置
     */
    void Initialize(
        APawn &InPawn,
        UCharacterMovementComponent &InMovement,
        USkeletalMeshComponent &InCharacterMesh,
        FBBBFacingRuntimeData &InFacingData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBAimRuntimeData &InAimData,
        const FBBBCharacterFacingConfig &InConfig);

    APawn *Pawn = nullptr;
    UCharacterMovementComponent *Movement = nullptr;
    USkeletalMeshComponent *CharacterMesh = nullptr;
    FBBBFacingRuntimeData *FacingData = nullptr;
    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;
    const FBBBIntentRuntimeData *IntentData = nullptr;
    const FBBBAimRuntimeData *AimData = nullptr;
    const FBBBCharacterFacingConfig *Config = nullptr;
};
