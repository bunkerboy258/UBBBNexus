#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Animation/BBBMeleeAnimInstance.h"
int32 UBBBMeleeAnimInstance::GetAttackSequence() const
{
    return AttackSequence;
}
bool UBBBMeleeAnimInstance::IsAttacking() const
{
    return bAttacking;
}
