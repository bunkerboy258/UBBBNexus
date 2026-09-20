#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/State/BBBCharacterInputState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/State/BBBCharacterParseState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterLocomotionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkState.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Input/BBBPlayerCameraInput.h"
#include "BBBCharacterRuntimeData.generated.h"
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;
class FBBBCharacterUpdate;
class FBBBCharacterAnimationFactProcessor;
class ABBBCharacter;
class UBBBAnimInstance;

struct FBBBCharacterRuntimeData;

namespace BBBCharacterInput
{
    template<typename TPacket>
    bool Submit(FBBBCharacterRuntimeData &Data, TPacket &&Packet);
}

USTRUCT()
//角色全部运行数据的唯一根
//输入区保存全部固定槽位 状态区发布解析结果并对所有系统与控制器开放读 领域数据为驻留系统的私有状态
struct FBBBCharacterRuntimeData
{
    GENERATED_BODY()

    /**
     * 在帧末清除禁止跨帧保留的数据
     */
    void Clean();

private:
    friend class ABBBCharacter;
    friend class ABBBPlayerCameraSystem;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterInputProcessor;
    friend class FBBBCharacterAnimationFactProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterShutdown;
    friend class FBBBCharacterUpdate;
    friend class UBBBAnimInstance;

    template<typename TPacket>
    friend bool BBBCharacterInput::Submit(FBBBCharacterRuntimeData &Data, TPacket &&Packet);

    // ===== 输入区 固定槽位仅由提交闸口与解析系统写入 =====

    /**
     * 全部输入数据的固定驻留区
     *
     * 同类型输入覆盖旧值，解析前后不搬运数组，也不为单个输入申请堆内存
     */
    FBBBCharacterInputState InputState;

    // ===== 状态区 解析后包应用效果的区域 对所有系统与控制器开放读 =====

    //保存当前帧世界时间快照
    UPROPERTY(Transient)
    FBBBCharacterWorldRuntimeData WorldData;

    //保存角色行为意图
    UPROPERTY(Transient)
    FBBBCharacterControlState Control;

    //保存角色瞄准状态
    UPROPERTY(Transient)
    FBBBAimRuntimeData Aim;

    //保存动画命令与处理状态
    UPROPERTY(Transient)
    FBBBAnimationState Animation;

    //保存相机命令与状态
    UPROPERTY(Transient)
    TOptional<FBBBPlayerCameraInput> CameraInput;

    //保存角色物品状态与命令
    UPROPERTY(Transient)
    FBBBCharacterEquipmentRuntimeData Equipment;

    //保存角色本地计算或网络恢复后的移动步态
    UPROPERTY(Transient)
    FBBBCharacterLocomotionRuntimeData Locomotion;

    //保存角色网络队列与观测状态
    UPROPERTY(Transient)
    FBBBNetworkState Network;

    // ===== 领域数据 解析系统私有 驻留黑板仅为生命周期托管 =====

    //保存动作请求与仲裁结果
    UPROPERTY(Transient)
    FBBBCharacterParseState Operation;
};
