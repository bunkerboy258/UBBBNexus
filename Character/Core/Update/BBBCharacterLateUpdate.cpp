#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterUpdatePipeline.h"

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

    Pipeline->LateUpdate();
}
