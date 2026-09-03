
#pragma once
#include "CoreMinimal.h"
class APawn;
class USpringArmComponent;
class FBBBCharacterInitializer;
struct FBBBCharacterCameraConfig;
struct FBBBCharacterWorldRuntimeData;
struct FBBBCameraRuntimeData;
struct FBBBInputRuntimeData;
struct FBBBIntentRuntimeData;
struct FBBBCharacterEquipmentEvents;

class ABBB_EVAC_API FBBBCharacterCameraSystem final
{
public:

    /**
     * 每帧更新相机 处理视角输入瞄准臂长插值与后座力恢复
     */
    void Update();
private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化相机系统依赖与配置
     * @param InPawn	所属角色Pawn
     * @param InCameraBoom	相机弹簧臂组件
     * @param InCameraData	相机运行时数据
     * @param InWorldData	世界运行时数据
     * @param InInputData	输入运行时数据
     * @param InIntentData	意图运行时数据
     * @param InConfig	相机配置
     */
    void Initialize(
        APawn &InPawn,
        USpringArmComponent &InCameraBoom,
        FBBBCameraRuntimeData &InCameraData,
        const FBBBCharacterWorldRuntimeData &InWorldData,
        const FBBBInputRuntimeData &InInputData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterEquipmentEvents &InEquipmentEvents,
        const FBBBCharacterCameraConfig &InConfig);

    APawn *Pawn = nullptr;

    USpringArmComponent *CameraBoom = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    const FBBBInputRuntimeData *InputData = nullptr;

    const FBBBIntentRuntimeData *IntentData = nullptr;

    const FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;

    FBBBCameraRuntimeData *CameraData = nullptr;

    const FBBBCharacterCameraConfig *Config = nullptr;
};
