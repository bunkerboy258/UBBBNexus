#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/AnimationSystem/Processors/BBBRiflePoseProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/Core/Update/BBBRifleUpdateContext.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Config/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Animation/BBBEquipmentAnimationFacts.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBRiflePoseProcessor::Update(FBBBRifleUpdateContext &Context)
{
    auto &Facts = Context.RuntimeData.Animation.AnimationState.Pose;
    // 失败时清除目标 避免动画继续使用上一帧的握持位置
    Facts.LeftHandTargetHandRSpace = FVector::ZeroVector;
    Facts.bHasLeftHandTarget = false;
    if (!ensureMsgf(IsInGameThread(), TEXT("步枪左手握持目标只能在游戏线程计算")))
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
        TEXT("步枪配置 %s 的左手握持 Socket %s 不存在于网格 %s"),
        *Context.Definition.GetName(),
        *SocketName.ToString(),
        *GetNameSafe(Context.WeaponMesh.GetSkeletalMeshAsset())))
    {
        return;
    }

    // 根的 Tick 依赖保证两侧网格已完成更新 动画线程仅消费随后发布的快照
    const FTransform SocketWorld = Context.WeaponMesh.GetSocketTransform(SocketName, RTS_World);
    const FVector TargetWorld = SocketWorld.TransformPosition(Context.Definition.LeftHandSocketOffset);
    const FVector TargetHandRSpace = HandWorld.InverseTransformPosition(TargetWorld)
        + Context.Definition.LeftHandIKOffset;
    if (!ensureMsgf(
        !TargetHandRSpace.ContainsNaN(),
        TEXT("步枪配置 %s 产生非有限左手握持目标"),
        *Context.Definition.GetName()))
    {
        return;
    }

    Facts.LeftHandTargetHandRSpace = TargetHandRSpace;
    Facts.bHasLeftHandTarget = true;
}
