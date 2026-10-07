#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/BBBMeleeActionSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeActionProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ActionSystem/Processors/BBBMeleeContactProcessor.h"
void FBBBMeleeActionSystem::Update(FBBBMeleeUpdateContext &Context)
{
    FBBBMeleeActionProcessor::Update(Context);
    FBBBMeleeContactProcessor::Update(Context);
}
