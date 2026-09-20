#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterLateUpdate::ExecuteTick(
    float DeltaTime,
    ELevelTick TickType,
    ENamedThreads::Type CurrentThread,
    const FGraphEventRef &MyCompletionGraphEvent)
{
    // 引擎延迟更新必须拥有有效角色目标
    if (!Target)
    {
        return;
    }

    // 将移动完成后的回调转交角色对象
    Target->LateUpdate();
}

//------------------------------------------------------------------------------

FString FBBBCharacterLateUpdate::DiagnosticMessage()
{
    return FString();
}

//------------------------------------------------------------------------------

FName FBBBCharacterLateUpdate::DiagnosticContext(bool bDetailed)
{
    static_cast<void>(bDetailed);
    return NAME_None;
}
