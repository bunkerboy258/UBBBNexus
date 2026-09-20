#pragma once

#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

class ABBBCharacter;
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;

/** 角色根管线只选择更新路径并调度阶段 */
class FBBBCharacterUpdatePipeline final
{
private:
    friend class ABBBCharacter;
    friend class FBBBCharacterInitializer;
    friend struct FBBBCharacterLateUpdate;
    friend class FBBBCharacterShutdown;

    void Initialize(ABBBCharacter &InCharacter);

    void RegisterTickFunctions(ABBBCharacter &InCharacter, bool bRegister);

    void Update(float DeltaSeconds) const;

    /** 调度移动完成后的动画事实更新 */
    void LateUpdate() const;

    ABBBCharacter *Character = nullptr;

    FBBBCharacterLateUpdate LateUpdateTick;
};
