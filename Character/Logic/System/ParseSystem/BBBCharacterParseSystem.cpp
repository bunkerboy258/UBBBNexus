#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/BBBCharacterParseSystem.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterParseSystem::Initialize(
    FBBBCharacterRuntimeData &InData,
    USkeletalMeshComponent &InCharacterMesh)
{
    Data = &InData;
    CharacterMesh = &InCharacterMesh;
}

void FBBBCharacterParseSystem::Update() const
{
    if (!Data || !CharacterMesh)
    {
        return;
    }

    UBBBAnimInstance *AnimationInstance = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    ensureMsgf(AnimationInstance, TEXT("角色输入解析缺少 BBB 动画实例 蒙太奇输入将在本帧被拒绝"));

    FBBBCharacterInputContext Context{
        AnimationInstance,
        Data->Aim.AimState,
        Data->Locomotion.LocomotionState,
        Data->Parse.ControlState,
        Data->Parse.CameraState.PendingInput};
    InputProcessor.Update(Data->Parse.InputState, Context);
}
