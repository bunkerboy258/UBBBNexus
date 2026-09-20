#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBCharacterLateUpdate::ExecuteTick(
    float DeltaTime,
    ELevelTick TickType,
    ENamedThreads::Type CurrentThread,
    const FGraphEventRef &MyCompletionGraphEvent)
{
    ABBBCharacter *Character = Cast<ABBBCharacter>(Target);
    if (!Character)
    {
        return;
    }

    // 将移动完成后的回调转交角色对象
    Character->LateUpdate();
}
