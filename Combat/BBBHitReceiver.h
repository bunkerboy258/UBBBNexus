#pragma once

#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "FBBBHitContribution.h"
#include "BBBHitReceiver.generated.h"

UINTERFACE()
class ABBB_EVAC_API UBBBHitReceiver : public UInterface
{
    GENERATED_BODY()
};

/** 不具有角色黑板的原生受击对象 */
class ABBB_EVAC_API IBBBHitReceiver
{
    GENERATED_BODY()

public:
    /** @param Hit 完整单次命中贡献 @return 无 */
    virtual void ReceiveHit(const FBBBHitContribution& Hit) = 0;
};

/** 普通模拟道具的目标侧接收器 现场由首个物理贡献装配 */
UCLASS()
class ABBB_EVAC_API UBBBPhysicsHitReceiver final : public UActorComponent, public IBBBHitReceiver
{
    GENERATED_BODY()

public:
    virtual void ReceiveHit(const FBBBHitContribution& Hit) override;
};

/** 向目标公开入口投递单次作用 */
namespace BBBHitReceiver
{
    /** @param Target 受击目标 @param Hit 完整单次命中贡献 @return 无 */
    ABBB_EVAC_API void Submit(AActor* Target, const FBBBHitContribution& Hit);
}
