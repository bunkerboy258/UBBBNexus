#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterLateUpdate::ExecuteTick(
    float DeltaTime,
    ELevelTick TickType,
    ENamedThreads::Type CurrentThread,
    const FGraphEventRef &MyCompletionGraphEvent)
{
    // 引擎延迟更新必须拥有有效角色目标
    if (!ensureMsgf(Target, TEXT("[UBBBC]Character LateUpdate failed because target is null")))
    {
        return;
    }

    // 将移动完成后的回调转交角色对象
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
