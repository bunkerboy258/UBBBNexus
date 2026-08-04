#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/States/BBBCharacterAnimationStates.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/BBBCharacterEquipmentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBCharacterRuntimeData.generated.h"
class FBBBCharacterInitializer;
class ABBBCharacter;

USTRUCT()
//角色全部运行数据的唯一根
struct FBBBCharacterRuntimeData
{
    GENERATED_BODY()

    /**
     * 在帧末清除禁止跨帧保留的数据
     */
    void Clean();

    /** @return 动画蓝图读取的最终表现状态 */
    const FBBBCharacterAnimationState &GetAnimationState() const
    {
        return AnimationState;
    }

private:
    friend class ABBBCharacter;
    friend class FBBBCharacterInitializer;

    //保存当前帧世界时间快照
    UPROPERTY(Transient)
    FBBBCharacterWorldRuntimeData WorldData;

    //保存原始输入与处理后输入
    UPROPERTY(Transient)
    FBBBInputRuntimeData Input;

    //保存角色行为意图
    UPROPERTY(Transient)
    FBBBIntentRuntimeData Intent;

    //保存动作请求与仲裁结果
    UPROPERTY(Transient)
    FBBBDecisionRuntimeData Decision;

    //保存角色朝向状态
    UPROPERTY(Transient)
    FBBBFacingRuntimeData Facing;

    //保存角色瞄准状态
    UPROPERTY(Transient)
    FBBBAimRuntimeData Aim;

    //保存动画命令与处理状态
    UPROPERTY(Transient)
    FBBBAnimationRuntimeData Animation;

    //保存动画蓝图最终表现状态
    UPROPERTY(Transient)
    FBBBCharacterAnimationState AnimationState;

    //保存相机命令与状态
    UPROPERTY(Transient)
    FBBBCameraRuntimeData Camera;

    //保存角色物品状态与命令
    UPROPERTY(Transient)
    FBBBCharacterEquipmentRuntimeData Equipment;

    //保存角色网络队列与观测状态
    UPROPERTY(Transient)
    FBBBNetworkRuntimeData Network;
};
