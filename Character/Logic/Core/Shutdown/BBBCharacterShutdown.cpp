#include "BBBWork/UBBBNexus/Character/Logic/Core/Shutdown/BBBCharacterShutdown.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterShutdown::Shutdown(ABBBCharacter &Character)
{
    // 先停止移动完成后的更新避免收束期间访问即将销毁的装备
    Character.CharacterUpdatePipeline.LateUpdateTick.SetTickFunctionEnable(false);

    Character.EquipmentSystem.Shutdown();
    Character.ItemSystem.Shutdown();
}
