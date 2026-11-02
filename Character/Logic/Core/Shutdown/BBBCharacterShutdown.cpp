#include "BBBWork/UBBBNexus/Character/Logic/Core/Shutdown/BBBCharacterShutdown.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PhysicalPresentationSystem/Processors/BBBCharacterHitReactionComponent.h"

void FBBBCharacterShutdown::Shutdown(ABBBCharacter &Character)
{
    // 先停止移动完成后的更新避免收束期间访问即将销毁的装备
    Character.CharacterUpdatePipeline.LateUpdateTick.SetTickFunctionEnable(false);
    Character.CharacterUpdatePipeline.PostPhysicsTick.SetTickFunctionEnable(false);

    if (Character.GetMesh() && Character.HitReaction)
    {
        Character.HitReaction->BindBodyContacts(*Character.GetMesh(), false);
    }
    Character.InteractionSystem.Shutdown();
    Character.AppearanceSystem.Shutdown();
    Character.ItemSystem.Shutdown();
}
