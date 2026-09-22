#pragma once

#include "CoreMinimal.h"

#include "MonsterDamageRequest.generated.h"

class AActor;
class AController;

/** 一次外部伤害输入 弱引用不会延长伤害来源的生命周期 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FMonsterDamageRequest
{
    GENERATED_BODY()

    /** 本次请求的正伤害量 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Damage")
    float Damage = 0.0f;

    /** 造成伤害的武器或其它演员 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Damage")
    TWeakObjectPtr<AActor> DamageCauser;

    /** 负责本次伤害的控制器 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Damage")
    TWeakObjectPtr<AController> Instigator;

    /** 世界空间命中位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Damage")
    FVector HitLocation = FVector::ZeroVector;

    /** 世界空间伤害传入方向 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Damage")
    FVector HitDirection = FVector::ZeroVector;
};
