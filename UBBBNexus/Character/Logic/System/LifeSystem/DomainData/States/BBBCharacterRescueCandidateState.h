#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BBBCharacterRescueCandidateState.generated.h"

/** 角色救援领域的独立状态 */
USTRUCT()
struct FBBBCharacterRescueCandidateState final
{
    GENERATED_BODY()

    /** 最近的合格倒地目标 */
    TWeakObjectPtr<APawn> Target;

    /** 本帧距离与视线均合格的对方角色 */
    TArray<TWeakObjectPtr<APawn>> EligiblePartners;

    /** 当前空间可以容纳恢复后的胶囊体 */
    bool bCanRecover = false;

    /** 当前空间只允许恢复为蹲姿 */
    bool bRecoverCrouched = false;

};
