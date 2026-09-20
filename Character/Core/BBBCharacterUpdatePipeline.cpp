#include "BBBWork/UBBBNexus/Character/Core/BBBCharacterUpdatePipeline.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterUpdatePipeline::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
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
