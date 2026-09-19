#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInput.h"

void FBBBCharacterInput::Initialize(FBBBCharacterInputRuntimeData &InData)
{
    Data = &InData;
}

bool FBBBCharacterInput::Submit(const FBBBCharacterInputStates &States)
{
    // 仅游戏线程可提交 状态必须通过自检
    if (!ensureMsgf(IsInGameThread() && Data && States.Movement.IsValid() && States.Aim.IsValid(),
        TEXT("[UBBBC]Invalid continuous states")))
    {
        return false;
    }

    Data->States = States;
    Data->States.Movement.Sanitize();
    return true;
}
