#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/AnimationSystem/Processors/BBBRevolverPoseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ActionSystem/DomainData/Context/BBBRevolverUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/RuntimeData/BBBRevolverRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/BBBRevolverEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Config/BBBRevolverDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimationFacts.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBRevolverPoseProcessor::Update(FBBBRevolverUpdateContext &Context)
{
    auto &Facts = Context.RuntimeData.Animation.AnimationState.Pose;
    // 失败时清除目标 避免动画继续使用上一帧的握持位置
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    Facts.LeftHandTargetHandRSpaceRotation = FRotator::ZeroRotator;
    Facts.bHasLeftHandTarget = false;
    if (!ensureMsgf(IsInGameThread(), TEXT("左轮左手握持目标只能在游戏线程计算")))
    {
        return;
    }

    const FName SocketName = Context.Definition.LeftHandSocketName;
    if (SocketName.IsNone())
    {
        return;
    }

    FTransform HandWorld;
    if (!ensureMsgf(
        Context.Character.TryGetRightHandWorldTransform(HandWorld),
        TEXT("角色 %s 缺少左手握持转换所需的 hand_r 骨骼"),
        *Context.Character.GetName()))
    {
        return;
    }

    if (!ensureMsgf(
        Context.WeaponMesh.DoesSocketExist(SocketName),
        TEXT("左轮配置 %s 的左手握持 Socket %s 不存在于网格 %s"),
        *Context.Definition.GetName(),
        *SocketName.ToString(),
        *GetNameSafe(Context.WeaponMesh.GetSkeletalMeshAsset())))
    {
        return;
    }

    // 根的 Tick 依赖保证两侧网格已完成更新 动画线程仅消费随后发布的快照
    const FTransform SocketWorld = Context.WeaponMesh.GetSocketTransform(SocketName, RTS_World);
    const FVector TargetHandRSpace = HandWorld.InverseTransformPosition(SocketWorld.GetLocation())
        + Context.Definition.LeftHandIKOffset;
    if (!ensureMsgf(
        !Context.Definition.LeftHandIKRotation.ContainsNaN(),
        TEXT("左轮配置 %s 的左手握持旋转包含非有限数值"),
        *Context.Definition.GetName()))
    {
        return;
    }

    const FQuat TargetRotationHandRSpace = (HandWorld.GetRotation().Inverse()
        * SocketWorld.GetRotation()
        * Context.Definition.LeftHandIKRotation.Quaternion()).GetNormalized();
    if (!ensureMsgf(
        !TargetHandRSpace.ContainsNaN() && !TargetRotationHandRSpace.ContainsNaN() && TargetRotationHandRSpace.IsNormalized(),
        TEXT("左轮配置 %s 产生非有限左手握持目标"),
        *Context.Definition.GetName()))
    {
        return;
    }

    Facts.LeftHandTargetHandRSpace = TargetHandRSpace;
    Facts.LeftHandTargetHandRSpaceRotation = TargetRotationHandRSpace.Rotator();
    Facts.bHasLeftHandTarget = true;
}
