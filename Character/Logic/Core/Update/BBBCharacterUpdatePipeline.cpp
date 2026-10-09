#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

void FBBBCharacterUpdatePipeline::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;

    // 角色领域依赖完成装配后才允许 CMC 后阶段运行
    LateUpdateTick.SetTickFunctionEnable(true);
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::RegisterTickFunctions(
    ABBBCharacter &InCharacter,
    const bool bRegister)
{
    Character = &InCharacter;

    UCharacterMovementComponent *Movement = InCharacter.GetCharacterMovement();
    USkeletalMeshComponent *CharacterMesh = InCharacter.GetMesh();
    if (!Movement || !CharacterMesh)
    {
        return;
    }

    if (bRegister)
    {
        InCharacter.HitReaction->AddTickPrerequisiteComponent(CharacterMesh);
        InCharacter.PhysicalAnimation->AddTickPrerequisiteComponent(InCharacter.HitReaction);
        // 后更新与 CMC 同组并等待 CMC 完成本帧移动
        LateUpdateTick.bCanEverTick = true;
        LateUpdateTick.bStartWithTickEnabled = false;
        LateUpdateTick.TickGroup = TG_PrePhysics;
        LateUpdateTick.Target = &InCharacter;
        LateUpdateTick.Pipeline = this;
        LateUpdateTick.SetTickFunctionEnable(InCharacter.HasActorBegunPlay());
        LateUpdateTick.AddPrerequisite(Movement, Movement->PrimaryComponentTick);
        LateUpdateTick.RegisterTickFunction(InCharacter.GetLevel());

        // 骨骼网格必须等待动画事实在 LateUpdate 中提交完成
        CharacterMesh->PrimaryComponentTick.AddPrerequisite(&InCharacter, LateUpdateTick);
        return;
    }

    if (CleanupWaiter.IsValid() && CleanupSource.IsValid())
    {
        CleanupWaiter->PrimaryActorTick.RemovePrerequisite(CleanupSource.Get(), CleanupSource->PrimaryActorTick);
    }
    CleanupWaiter.Reset();
    CleanupSource.Reset();

    // 注销时按反向顺序移除依赖与 Tick 注册
    InCharacter.PhysicalAnimation->RemoveTickPrerequisiteComponent(InCharacter.HitReaction);
    InCharacter.HitReaction->RemoveTickPrerequisiteComponent(CharacterMesh);
    CharacterMesh->PrimaryComponentTick.RemovePrerequisite(&InCharacter, LateUpdateTick);
    LateUpdateTick.RemovePrerequisite(Movement, Movement->PrimaryComponentTick);
    LateUpdateTick.UnRegisterTickFunction();
    LateUpdateTick.Pipeline = nullptr;
    LateUpdateTick.Target = nullptr;
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::Update(const float DeltaSeconds)
{
    if (!Character)
    {
        return;
    }

    const UWorld *World = Character->GetWorld();
    if (!World)
    {
        return;
    }

    // 移除上一帧装备切换产生的临时 Tick 依赖
    if (CleanupWaiter.IsValid() && CleanupSource.IsValid())
    {
        CleanupWaiter->PrimaryActorTick.RemovePrerequisite(CleanupSource.Get(), CleanupSource->PrimaryActorTick);
    }
    CleanupWaiter.Reset();
    CleanupSource.Reset();

    // 帧上下文：所有领域系统读取同一份时间与网络身份快照
    FBBBCharacterWorldState &WorldState = Character->RuntimeData.External.WorldState;
    WorldState.FrameDeltaSeconds = DeltaSeconds;
    WorldState.WorldTimeSeconds = World->GetTimeSeconds();

    FBBBCharacterNetworkIdentityState &NetworkIdentityState =
        Character->RuntimeData.External.NetworkIdentityState;
    NetworkIdentityState.bHasAuthority = Character->HasAuthority();
    NetworkIdentityState.bLocallyControlled = Character->IsLocallyControlled();
    NetworkIdentityState.bIsMirror = !NetworkIdentityState.bLocallyControlled;

    // 输入与物品：先解析请求，再更新物品、装备关系和外观
    Character->ParseSystem.Update();
    if (!NetworkIdentityState.bIsMirror)
    {
        Character->ItemSystem.Update();
    }

    ABBBEquipment *PreviousEquipment = Character->GetActiveEquipment();
    Character->EquipmentSystem.Update();
    Character->AppearanceSystem.Update();

    // 装备切换帧：新装备等待旧装备完成本帧 Tick
    ABBBEquipment *CurrentEquipment = Character->GetActiveEquipment();
    if (IsValid(PreviousEquipment) &&
        IsValid(CurrentEquipment) &&
        PreviousEquipment != CurrentEquipment &&
        PreviousEquipment->IsActorTickEnabled())
    {
        CurrentEquipment->PrimaryActorTick.AddPrerequisite(PreviousEquipment, PreviousEquipment->PrimaryActorTick);
        CleanupWaiter = CurrentEquipment;
        CleanupSource = PreviousEquipment;
    }

    // 生命与瞄准：只有本机控制角色根据输入产生新的瞄准事实
    Character->LifeSystem.Update();

    if (!NetworkIdentityState.bIsMirror)
    {
        Character->AimSystem.Update();
    }

    // 攀爬只生成交接结果 移动系统独占 CMC 写入
    Character->TraversalSystem.Update();

    // 操作许可位于攀爬决策后与 CMC 前 不重复装备关系维护
    Character->EquipmentSystem.UpdateUsage();

    // 更新移动与物理表现
    Character->LocomotionSystem.Update();
    Character->PhysicalPresentationSystem.Update();

    // 网络只观察已经成立的状态与事实
    Character->NetworkSystem.Update();
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::LateUpdate() const
{
    if (!Character)
    {
        return;
    }

    // CMC 结束后采集最终移动结果并更新动画事实
    Character->AnimationSystem.Update();

}

void FBBBCharacterUpdatePipeline::RegisterEquipmentTicks(USkeletalMeshComponent &Mesh, ABBBEquipment &Equipment)
{
    Equipment.PrimaryActorTick.AddPrerequisite(Mesh.GetOwner(), Mesh.GetOwner()->PrimaryActorTick);
    Equipment.PrimaryActorTick.AddPrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    if (auto *EquipmentMesh = Equipment.GetEquipmentSkeletalMesh())
    {
        EquipmentMesh->PrimaryComponentTick.AddPrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    }
}

void FBBBCharacterUpdatePipeline::UnregisterEquipmentTicks(USkeletalMeshComponent &Mesh, ABBBEquipment &Equipment)
{
    Equipment.PrimaryActorTick.RemovePrerequisite(Mesh.GetOwner(), Mesh.GetOwner()->PrimaryActorTick);
    Equipment.PrimaryActorTick.RemovePrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    if (auto *EquipmentMesh = Equipment.GetEquipmentSkeletalMesh())
    {
        EquipmentMesh->PrimaryComponentTick.RemovePrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    }
}
