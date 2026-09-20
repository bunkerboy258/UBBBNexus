#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdate.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterUpdate::Initialize(ABBBCharacter &InCharacter)
{
    Character = &InCharacter;
}

//------------------------------------------------------------------------------

void FBBBCharacterUpdate::Update() const
{
    if (!Character)
    {
        return;
    }

    const bool bAuthority = Character->HasAuthority();
    const bool bLocallyControlled = Character->IsLocallyControlled();

    // 输入必须先解析为角色黑板状态
    Character->ParseSystem.Update();

    // 装备动作可能生成需要网络观察的离散事实
    Character->EquipmentController.Update();

    // 只有权威与本机控制角色可以生成瞄准和移动事实
    if (bAuthority || bLocallyControlled)
    {
        Character->AimController.Update();
        Character->LocomotionController.Update();
    }

    // 网络系统只在全部领域事实已经成立后执行观察与传输
    Character->NetworkSystem.Update();
}
