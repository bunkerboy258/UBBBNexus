#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"

void FBBBCharacterLateUpdate::ExecuteTick(
    float DeltaTime,
    ELevelTick TickType,
    ENamedThreads::Type CurrentThread,
    const FGraphEventRef &MyCompletionGraphEvent)
{
    if (!Pipeline)
    {
        return;
    }

    // 引擎回调只转发到管线 不承载领域逻辑
    Pipeline->LateUpdate();
}
