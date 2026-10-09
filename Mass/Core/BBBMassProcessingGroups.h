#pragma once

#include "CoreMinimal.h"

/** 项目实体更新阶段 */
namespace BBBMassProcessingGroups
{
    inline const FName Parse = TEXT("BBBMassParse");
    inline const FName Initialization = TEXT("BBBMassInitialization");
    inline const FName Decision = TEXT("BBBMassDecision");
    inline const FName Movement = TEXT("BBBMassMovement");
    inline const FName Collision = TEXT("BBBMassCollision");
    inline const FName Damage = TEXT("BBBMassDamage");
    inline const FName Network = TEXT("BBBMassNetwork");
    inline const FName Presentation = TEXT("BBBMassPresentation");
    inline const FName Lifetime = TEXT("BBBMassLifetime");
}
