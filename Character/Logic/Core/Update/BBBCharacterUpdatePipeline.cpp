#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
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
    PostPhysicsTick.SetTickFunctionEnable(true);
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
        // 物理完成后读取刚体 不从 PrePhysics 动画缓存推断本帧物理位置
        PostPhysicsTick.bCanEverTick = true;
        PostPhysicsTick.bStartWithTickEnabled = false;
        PostPhysicsTick.TickGroup = TG_PostPhysics;
        PostPhysicsTick.EndTickGroup = TG_PostPhysics;
        PostPhysicsTick.Target = &InCharacter;
        PostPhysicsTick.Pipeline = this;
        PostPhysicsTick.bAfterPhysics = true;
        PostPhysicsTick.SetTickFunctionEnable(InCharacter.HasActorBegunPlay());
        PostPhysicsTick.AddPrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
        PostPhysicsTick.RegisterTickFunction(InCharacter.GetLevel());

        // 骨骼网格必须等待动画事实在 LateUpdate 中提交完成
        CharacterMesh->PrimaryComponentTick.AddPrerequisite(&InCharacter, LateUpdateTick);
        return;
    }

    // 注销时按反向顺序移除依赖与 Tick 注册
    PostPhysicsTick.RemovePrerequisite(CharacterMesh, CharacterMesh->PrimaryComponentTick);
    PostPhysicsTick.UnRegisterTickFunction();
    PostPhysicsTick.Pipeline = nullptr;
    PostPhysicsTick.Target = nullptr;
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

    // 帧上下文：所有领域系统读取同一份时间与网络身份快照
    FBBBCharacterWorldState &WorldState = Character->RuntimeData.External.WorldState;
    WorldState.FrameDeltaSeconds = DeltaSeconds;
    WorldState.WorldTimeSeconds = World->GetTimeSeconds();

    FBBBCharacterNetworkIdentityState &NetworkIdentityState =
        Character->RuntimeData.External.NetworkIdentityState;
    NetworkIdentityState.bHasAuthority = Character->HasAuthority();
    NetworkIdentityState.bLocallyControlled = Character->IsLocallyControlled();
    NetworkIdentityState.bIsMirror = !NetworkIdentityState.bLocallyControlled;

    // 两条路径都先解析输入，再根据本帧身份执行对应更新
    Character->ParseSystem.Update();

    if (NetworkIdentityState.bIsMirror)
    {
        Character->InteractionSystem.Update();
        Character->AppearanceSystem.Update();
        Character->LifeSystem.Update();
        Character->PhysicalPresentationSystem.Update();
        Character->TraversalSystem.Update();
        Character->InteractionSystem.UpdateUsage();
        Character->LocomotionSystem.Update();
    }
    else
    {
        Character->ItemSystem.Update();
        Character->InteractionSystem.Update();
        Character->AppearanceSystem.Update();
        Character->LifeSystem.Update();
        Character->PhysicalPresentationSystem.Update();
        Character->AimSystem.Update();
        Character->PickupSystem.Update();
        Character->TraversalSystem.Update();
        Character->InteractionSystem.UpdateUsage();
        Character->LocomotionSystem.Update();
    }

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
    Character->LocomotionSystem.CaptureMovementFacts();
    Character->AnimationSystem.Update();

}

void FBBBCharacterUpdatePipeline::PostPhysicsUpdate() const
{
    if (!Character)
    {
        return;
    }
    Character->PhysicalPresentationSystem.CapturePhysics();
    Character->LocomotionSystem.FollowRagdoll();
}

void FBBBCharacterUpdatePipeline::RegisterItemTicks(USkeletalMeshComponent &Mesh, AActor &Item)
{
    Item.PrimaryActorTick.AddPrerequisite(Mesh.GetOwner(), Mesh.GetOwner()->PrimaryActorTick);
    Item.PrimaryActorTick.AddPrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    if (auto *Equipment = Cast<ABBBEquipment>(&Item))
    {
        auto *EquipmentMesh = Equipment->GetEquipmentSkeletalMesh();
        if (EquipmentMesh)
        {
            EquipmentMesh->PrimaryComponentTick.AddPrerequisite(&Mesh, Mesh.PrimaryComponentTick);
        }
    }
}

void FBBBCharacterUpdatePipeline::RegisterCameraTicks(ABBBCharacter &InCharacter, AActor &Camera)
{
    Camera.PrimaryActorTick.AddPrerequisite(&InCharacter, InCharacter.CharacterUpdatePipeline.PostPhysicsTick);
}

void FBBBCharacterUpdatePipeline::UnregisterItemTicks(USkeletalMeshComponent &Mesh, AActor &Item)
{
    Item.PrimaryActorTick.RemovePrerequisite(Mesh.GetOwner(), Mesh.GetOwner()->PrimaryActorTick);
    Item.PrimaryActorTick.RemovePrerequisite(&Mesh, Mesh.PrimaryComponentTick);
    if (auto *Equipment = Cast<ABBBEquipment>(&Item))
    {
        auto *EquipmentMesh = Equipment->GetEquipmentSkeletalMesh();
        if (EquipmentMesh)
        {
            EquipmentMesh->PrimaryComponentTick.RemovePrerequisite(&Mesh, Mesh.PrimaryComponentTick);
        }
    }
}
