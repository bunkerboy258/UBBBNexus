#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/Processors/BBBCharacterRagdollFollowProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/Context/BBBCharacterMovementFactContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "GameFramework/Character.h"

void FBBBCharacterRagdollFollowProcessor::Update(FBBBCharacterMovementFactContext &Context) const
{
    const auto &Physical = Context.RuntimeData.PhysicalPresentation.ReadPhysicalPresentationState();
    if (!Physical.bRagdoll)
    {
        return;
    }
    // 胶囊已无碰撞 网格已脱离 父组件移动不会拖拽刚体
    Context.Character.SetActorLocation(Physical.PelvisLocation, false, nullptr, ETeleportType::TeleportPhysics);
}
