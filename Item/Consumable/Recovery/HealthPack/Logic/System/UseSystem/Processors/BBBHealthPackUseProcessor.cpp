#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/Processors/BBBHealthPackUseProcessor.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/DomainData/Context/BBBHealthPackUseUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/RuntimeData/BBBHealthPackRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Config/BBBHealthPackDefinition.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
void FBBBHealthPackUseProcessor::Update(FBBBHealthPackUseUpdateContext &Context) const
{
    auto &State = Context.Data.Use.UseState;
    auto &Inputs = Context.Data.Use.UseInputState;
    if (!Inputs.bPrimary || Inputs.bFinish || State.Stage == Inputs.Stage)
    {
        return;
    }
    UAnimMontage *Montage = Context.Definition.UseMontage;
    if (!Montage)
    {
        Inputs.bFinish = true;
        return;
    }
    State.Stage = Inputs.Stage;
    State.bUsing = true;
    Context.Character.SubmitInput(FBBBUpperBodyMontageLocalControlPacket{Montage});
}
