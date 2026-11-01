#pragma once

#include "GameFramework/Actor.h"
#include "BBBSlimePresentationActor.generated.h"

class UStaticMeshComponent;
struct FBBBSlimeFact;

/** 粘液的无碰撞机械显示载体 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBSlimePresentationActor final : public AActor
{
    GENERATED_BODY()
public:
    ABBBSlimePresentationActor();
    /** @param Fact 当前飞行或地面结果 @return 只更新机械显示 */
    void ApplyFact(const FBBBSlimeFact& Fact);
private:
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> Body;
};
