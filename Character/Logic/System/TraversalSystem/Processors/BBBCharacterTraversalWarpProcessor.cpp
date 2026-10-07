#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/Processors/BBBCharacterTraversalWarpProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/Context/BBBCharacterTraversalUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"

void FBBBCharacterTraversalWarpProcessor::Update(FBBBCharacterTraversalUpdateContext &Context) const
{
    FBBBCharacterTraversalState &State = Context.Traversal;
    if (State.Action == EBBBTraversalAction::None && State.bTargetsInstalled)
    {
        // 仅退役攀爬命名窗口 标记移除避免下次播放复用 Disabled 的旧实例
        for (URootMotionModifier *Modifier : Context.Warping.GetModifiers())
        {
            URootMotionModifier_Warp *Warp = Cast<URootMotionModifier_Warp>(Modifier);
            if (Warp && (Warp->WarpTargetName == TEXT("TraversalContact") || Warp->WarpTargetName == TEXT("TraversalEnd")))
            {
                Warp->SetState(ERootMotionModifierState::MarkedForRemoval);
            }
        }
        Context.Warping.RemoveWarpTarget(TEXT("TraversalContact"));
        Context.Warping.RemoveWarpTarget(TEXT("TraversalEnd"));
        State.bTargetsInstalled = false;
        return;
    }

    if (State.Action != EBBBTraversalAction::None)
    {
        // 退出请求不撤销目标 动画确认停止根运动后再执行定向清理
        Context.Warping.AddOrUpdateWarpTargetFromTransform(TEXT("TraversalContact"), State.ContactTarget);
        Context.Warping.AddOrUpdateWarpTargetFromTransform(TEXT("TraversalEnd"), State.EndTarget);
        State.bTargetsInstalled = true;
    }
}
