#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterControlInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBCharacterControlInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return !MoveWorld.ContainsNaN() && !FacingWorld.ContainsNaN() && !AimTargetWorld.ContainsNaN();
}

void FBBBCharacterControlInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    Data.Operation.Control = *this;
}

void FBBBCharacterControlInput::FinishFrame(FBBBCharacterRuntimeData &Data)
{
    FBBBCharacterControlInput &Control = Data.Operation.Control;

    // 瞄准或已获准开火时冲刺必须退出 裁决后的事实一次写入控制黑板
    Control.bSprint = Control.bSprint && AllowsSprint(Control.bAim || Control.bFire);
    Data.Control.Value = Control;
}

bool FBBBCharacterControlInput::AllowsSprint(const bool bAiming)
{
    return !bAiming;
}
