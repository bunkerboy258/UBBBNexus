#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    // 注销时按反向顺序移除依赖与 Tick 注册
    CharacterMesh->PrimaryComponentTick.RemovePrerequisite(&InCharacter, LateUpdateTick);
    LateUpdateTick.RemovePrerequisite(Movement, Movement->PrimaryComponentTick);
    LateUpdateTick.UnRegisterTickFunction();
    LateUpdateTick.Pipeline = nullptr;
    LateUpdateTick.Target = nullptr;
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdatePipeline::Update(const float DeltaSeconds) const
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

    // 所有领域系统读取同一份本帧世界时间快照
    Character->RuntimeData.WorldData.FrameDeltaSeconds = DeltaSeconds;
    Character->RuntimeData.WorldData.WorldTimeSeconds = World->GetTimeSeconds();
    Character->RuntimeData.NetworkIdentity.Refresh(
        Character->HasAuthority(),
        Character->IsLocallyControlled());

    Character->ParseSystem.Update();
    Character->EquipmentController.Update(Character->RuntimeData.NetworkIdentity);

    if (Character->RuntimeData.NetworkIdentity.ExecutionMode
        == EBBBCharacterExecutionMode::Causal)
    {
        // 只有本机控制角色可以根据控制输入产生新的瞄准与移动事实。
        Character->AimController.Update();
        Character->LocomotionController.Update();
    }

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

    // 清理不允许跨帧驻留的输入与瞬时数据
    Character->RuntimeData.Clean();
}
