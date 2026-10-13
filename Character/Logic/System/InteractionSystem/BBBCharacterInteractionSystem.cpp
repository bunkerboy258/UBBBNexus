#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/BBBCharacterInteractionSystem.h"

#include "BBBWork/UBBBNexus/Character/Config/Interaction/BBBCharacterInteractionConfig.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/BBBCharacterInteractionDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/Context/BBBCharacterInteractionUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterInteractionSystem::Initialize(
    USkeletalMeshComponent &InCharacterMesh,
    FBBBCharacterRuntimeData &InRuntimeData,
    ABBBCharacter &InCharacter,
    const FBBBCharacterInteractionConfig &InInteractionConfig)
{
    // 保存角色持有物品交互依赖与手部挂点配置
    CharacterMesh = &InCharacterMesh;
    RuntimeData = &InRuntimeData;
    Character = &InCharacter;
    RightHandSocketName = InInteractionConfig.RightHandSocketName;
}

void FBBBCharacterInteractionSystem::Update()
{
    // 持有物品更新需要角色网格和交互运行时数据有效
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }

    FBBBCharacterInteractionUpdateContext Context{
        *Character,
        *CharacterMesh,
        *RuntimeData,
        RightHandSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};

    SelectionProcessor.Update(Context);
    LifecycleProcessor.Update(Context);
}

void FBBBCharacterInteractionSystem::Shutdown()
{
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }
    FBBBCharacterInteractionUpdateContext Context{
        *Character, *CharacterMesh, *RuntimeData, RightHandSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};
    LifecycleProcessor.Shutdown(Context);
}

void FBBBCharacterInteractionSystem::UpdateUsage()
{
    if (!RuntimeData || !CharacterMesh || !Character)
    {
        return;
    }

    FBBBCharacterInteractionUpdateContext Context{
        *Character,
        *CharacterMesh,
        *RuntimeData,
        RightHandSocketName,
        RuntimeData->External.ReadNetworkIdentityState().bIsMirror};
    UseProcessor.Update(Context);
    ActionPermissionProcessor.Update(Context);
    AnimationInputProcessor.Update(Context);
}
