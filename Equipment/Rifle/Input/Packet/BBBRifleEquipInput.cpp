#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleEquipInput.h"

#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/DomainData/Context/BBBRifleInputContext.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Processors/BBBRiflePresentationProcessor.h"

bool FBBBRifleEquipInput::IsValid() const
{
    return Sequence != INDEX_NONE;
}

bool FBBBRifleEquipInput::CanApply(const FBBBRifleInputContext &Context) const
{
    return Context.Definition.CharacterEquipMontage != nullptr;
}

void FBBBRifleEquipInput::Apply(FBBBRifleInputContext &Context) const
{
    FBBBRiflePresentationProcessor::SubmitCharacterMontage(
        Context,
        Context.Definition.CharacterEquipMontage);
}
