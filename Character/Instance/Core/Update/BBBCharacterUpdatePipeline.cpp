#include "BBBWork/UBBBNexus/Character/Instance/Core/Update/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterUpdatePipeline::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
}

void FBBBCharacterUpdatePipeline::Update() const
{
    if (!ensureMsgf(Character, TEXT("[UBBBC]Character pipeline is uninitialized")))
    {
        return;
    }
    // 保留本地控制端完成因果的模型 网络身份仅在根管线与网络系统中判定
    const bool bRestoreMode = !Character->IsLocallyControlled();
    Character->NetworkSystem.UpdateRestore();
    Character->InputPipeline.Update();
    Character->ArbitrationPipeline.Update(bRestoreMode);
    Character->ExecutionPipeline.Update();
    Character->EquipmentSystem.Update();
    if (!bRestoreMode)
    {
        Character->AimSystem.Update();
        Character->LocomotionSystem.Update();
        Character->NetworkSystem.UpdateUpload();
    }
}

void FBBBCharacterUpdatePipeline::LateUpdate() const
{
    if (!ensureMsgf(Character, TEXT("[UBBBC]Character late pipeline is uninitialized")))
    {
        return;
    }
    // 移动完成后提交动画事实 镜像也需要表现更新但不生成新的玩法结果
    Character->AnimationSystem.Update();
    Character->RuntimeData.Clean();
}
