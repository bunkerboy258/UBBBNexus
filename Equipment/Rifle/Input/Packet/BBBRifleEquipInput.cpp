#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/Packet/BBBRifleEquipInput.h"

#include "BBBWork/UBBBNexus/Equipment/Rifle/Definition/BBBRifleDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Rifle/Input/BBBRifleInputContext.h"

bool FBBBRifleEquipInput::IsValid() const
{
    return Sequence != INDEX_NONE;
}

bool FBBBRifleEquipInput::CanApply(const FBBBRifleInputContext &Context) const
{
    return Context.Definition.EquipMontage != nullptr;
}

void FBBBRifleEquipInput::Apply(FBBBRifleInputContext &Context) const
{
    Context.SubmitMontage(Context.Definition.EquipMontage, Sequence, false);
}
