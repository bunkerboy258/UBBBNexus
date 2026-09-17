#include "BBBWork/UBBBNexus/Character/Instance/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterLateUpdate::ExecuteTick(
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

FString FBBBCharacterLateUpdate::DiagnosticMessage()
{
    return GetFullNameSafe(Target) + TEXT("[ABBBCharacter::LateUpdate]");
}

//------------------------------------------------------------------------------

FName FBBBCharacterLateUpdate::DiagnosticContext(bool bDetailed)
{
    if (bDetailed)
    {
        return FName(*FString::Printf(TEXT("CharacterLateUpdate/%s"), *GetFullNameSafe(Target)));
    }

    return FName(TEXT("CharacterLateUpdate"));
}
