#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterRestoreInput.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"

bool FBBBCharacterRestoreInput::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    return Data.Operation.bRestoreMode;
}

void FBBBCharacterRestoreInput::Apply(FBBBCharacterRuntimeData &Data) const
{
    // 镜像包已经包含权威端因果结果 这里只还原黑板而不重新触发本地输入
    if (bEquipmentChanged)
    {
        Data.Equipment.Commands.PendingRestoredEquipment = Equipment;
    }
    for (const FBBBEquipmentActionEvent &Action : Actions)
    {
        Data.Equipment.Commands.SubmitRestoredAction(Action);
    }
    if (Aim.IsSet())
    {
        Data.Aim.ApplyRestoredState(Aim.GetValue());
    }
    if (Gait.IsSet())
    {
        Data.Locomotion.CommitGait(Gait.GetValue());
    }
}
