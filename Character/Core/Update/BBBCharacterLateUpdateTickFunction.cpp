#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdateTickFunction.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterLateUpdateTickFunction::ExecuteTick(
    float DeltaTime,
    ELevelTick TickType,
    ENamedThreads::Type CurrentThread,
    const FGraphEventRef &MyCompletionGraphEvent)
{
    if (!ensureMsgf(Target, TEXT("[UBBBC]Character LateUpdate failed because target is null")))
    {
        return;
    }

    Target->LateUpdate();
}

//------------------------------------------------------------------------------

FString FBBBCharacterLateUpdateTickFunction::DiagnosticMessage()
{
    return GetFullNameSafe(Target) + TEXT("[ABBBCharacter::LateUpdate]");
}

//------------------------------------------------------------------------------

FName FBBBCharacterLateUpdateTickFunction::DiagnosticContext(bool bDetailed)
{
    if (bDetailed)
    {
        return FName(*FString::Printf(TEXT("CharacterLateUpdate/%s"), *GetFullNameSafe(Target)));
    }

    return FName(TEXT("CharacterLateUpdate"));
}
