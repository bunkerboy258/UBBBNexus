#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBLinkedAnimInstance.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"

UBBBAnimInstance *UBBBLinkedAnimInstance::GetBBBMainAnimInstanceThreadSafe() const
{
    return Cast<UBBBAnimInstance>(Blueprint_GetMainAnimInstance());
}
