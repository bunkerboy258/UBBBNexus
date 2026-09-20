#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"
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
    const bool bAuthority = Character->HasAuthority();
    const bool bLocallyControlled = Character->IsLocallyControlled();

    Character->ParseSystem.Update();
    Character->EquipmentController.Update();

    if (bAuthority || bLocallyControlled)
    {
        Character->AimController.Update();
        Character->LocomotionController.Update();
    }

    // 网络系统内部根据身份选择命令上传、事实广播或空远端阶段
    Character->NetworkSystem.Update();
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
