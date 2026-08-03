
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBCharacterAnimationState.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/CameraSystem/Definition/BBBCameraRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Request/Definition/BBBDecisionTypes.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/World/BBBCharacterWorldRuntimeData.h"
#include "BBBCharacterRuntimeData.generated.h"
class FBBBCharacterInitializer;
class ABBBCharacter;
struct FBBBCharacterRuntimeData;

USTRUCT()
//聚合各逻辑系统共享的角色运行时数据
struct FBBBCharacterData
{
    GENERATED_BODY()
private:
    //初始化器负责向各系统分发内部数据引用
    friend class FBBBCharacterInitializer;

    //根运行时数据负责统一清理单帧状态
    friend struct FBBBCharacterRuntimeData;

    //保存归一化后的持续输入与单帧输入
    UPROPERTY(Transient)
    FBBBInputRuntimeData Input;

    //保存动画命令与动画处理过程数据
    UPROPERTY(Transient)
    FBBBAnimationRuntimeData Animation;

    //保存由输入转换得到的角色行为意图
    UPROPERTY(Transient)
    FBBBIntentRuntimeData Intent;

    //保存角色当前朝向计算过程数据
    UPROPERTY(Transient)
    FBBBFacingRuntimeData Facing;

    //保存本地或远端瞄准状态
    UPROPERTY(Transient)
    FBBBAimRuntimeData Aim;

    //保存装备物品命令事件与状态
    UPROPERTY(Transient)
    FBBBCharacterItemRuntimeData Item;

    //保存相机输入与视角旋转数据
    UPROPERTY(Transient)
    FBBBCameraRuntimeData Camera;

    //保存动作请求仲裁结果与执行队列
    UPROPERTY(Transient)
    FBBBDecisionRuntimeData Decision;

    //保存待校验待恢复与待上传的网络数据
    UPROPERTY(Transient)
    FBBBNetworkRuntimeData Network;
};

USTRUCT()
//保存允许动画实例读取的最终表现数据
struct FBBBCharacterPresentationData
{
    GENERATED_BODY()

    /**
     * 向根运行时数据公开只读表现快照
     * @return 动画表现状态常量引用
     */
    const FBBBCharacterAnimationState &GetAnimationState() const
    {
        return AnimationState;
    }
private:
    //初始化器负责把表现数据注入动画系统
    friend class FBBBCharacterInitializer;

    //根运行时数据负责对外提供只读访问
    friend struct FBBBCharacterRuntimeData;

    //保存动画蓝图每帧消费的角色表现快照
    UPROPERTY(Transient)
    FBBBCharacterAnimationState AnimationState;
};

USTRUCT()
//拥有角色原始输入逻辑黑板与表现黑板
struct FBBBCharacterRuntimeData
{
    GENERATED_BODY()

    /**
     * 在一帧末尾清除禁止跨帧保留的瞬时数据
     */
    void Clean();

    /**
     * 向角色与动画实例公开只读表现快照
     * @return 最终表现数据而非中间逻辑状态
     */
    const FBBBCharacterAnimationState &GetAnimationState() const
    {
        //返回最终表现数据而非中间逻辑状态
        return CharacterPresentationData.GetAnimationState();
    }
private:
    friend class ABBBCharacter;

    //初始化器负责建立各子数据与系统之间的引用
    friend class FBBBCharacterInitializer;

    //直接接收增强输入回调写入的原始输入缓冲
    FBBBInputRawData InputRawData;

    /** 当前帧世界时间快照 */
    UPROPERTY(Transient)
    FBBBCharacterWorldRuntimeData WorldData;

    //集中保存角色各逻辑系统的共享数据
    UPROPERTY(Transient)
    FBBBCharacterData CharacterData;

    //隔离提供给动画层的稳定表现数据
    UPROPERTY(Transient)
    FBBBCharacterPresentationData CharacterPresentationData;
};
