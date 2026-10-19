#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/Processors/BBBSniperPoseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/AnimationSystem/DomainData/Context/BBBSniperAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/RuntimeData/BBBSniperRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/BBBSniperEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/BBBSniperDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Animation/BBBEquipmentAnimationFacts.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBSniperPoseProcessor::Update(FBBBSniperAnimationUpdateContext &Context)
{
    auto &Facts = Context.RuntimeData.Animation.AnimationState.Pose;
    // 失败时清除目标 避免动画继续使用上一帧的握持位置
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    Facts.bHasLeftHandTarget = false;
    if (!ensureMsgf(IsInGameThread(), TEXT("狙击枪左手握持目标只能在游戏线程计算")))
    {
        return;
    }

    const FName SocketName = Context.Definition.LeftHandSocketName;
    FTransform HandWorld;
    if (!ensureMsgf(
        Context.Character.TryGetRightHandWorldTransform(HandWorld),
        TEXT("角色 %s 缺少左手握持转换所需的 hand_r 骨骼"),
        *Context.Character.GetName()))
    {
        return;
    }

    if (!ensureMsgf(
        !SocketName.IsNone() && Context.WeaponMesh.DoesSocketExist(SocketName),
        TEXT("狙击枪配置 %s 的左手握持 Socket %s 不存在于网格 %s"),
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
        !TargetHandRSpace.ContainsNaN(),
        TEXT("狙击枪配置 %s 产生非有限左手握持目标"),
        *Context.Definition.GetName()))
    {
        return;
    }

    Facts.LeftHandTargetHandRSpace = TargetHandRSpace;
    Facts.bHasLeftHandTarget = true;
}
