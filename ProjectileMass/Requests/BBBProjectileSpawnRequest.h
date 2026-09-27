#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

class UBBBProjectileDefinition;

/** 从一次已确认的本地开火构造一枚弹丸 */
struct FBBBProjectileSpawnRequest final
{
    /** 本次开火使用的弹丸定义 */
    const UBBBProjectileDefinition* Definition = nullptr;

    /** 弹丸出生时的枪口世界变换 */
    FTransform MuzzleTransform = FTransform::Identity;

    /** 造成伤害的装备 */
    TWeakObjectPtr<AActor> DamageCauser;

    /** 发射弹丸的角色 */
    TWeakObjectPtr<APawn> InstigatorPawn;

    /** 伤害事件中的控制器来源 */
    TWeakObjectPtr<AController> EventInstigator;

    /** @return 弹丸定义和伤害来源是否有效 */
    bool IsValid() const;
};
