#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/Processors/BBBGrenadeUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/DomainData/Context/BBBGrenadeUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Config/BBBGrenadeDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
void FBBBGrenadeUseProcessor::Update(FBBBGrenadeUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (!Inputs.bPrimary || Inputs.bFinish || State.Stage == Inputs.Stage)
    {
        return;
    }
    UAnimMontage *Montage = Context.Definition.ThrowMontage;
    if (Inputs.Stage == 1)
    {
        Montage = Context.Definition.PrepareMontage;
    }
    if (Inputs.Stage == 2)
    {
        Montage = Context.Definition.HighHoldMontage;
    }
    if (Inputs.Stage == 3)
    {
        Montage = Context.Definition.LowHoldMontage;
    }
    if (Inputs.Stage == 5)
    {
        Montage = Context.Definition.LowThrowMontage;
    }
    if (!Montage)
    {
        Inputs.bFinish = true;
        return;
    }
    State.Stage = Inputs.Stage;
    State.bUsing = true;
    Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Montage});
}
