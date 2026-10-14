#pragma once
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/Launch/BBBThrowableLaunchConfig.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/Physics/BBBThrowablePhysicsConfig.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/Explosion/BBBThrowableExplosionConfig.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Config/Presentation/BBBThrowablePresentationConfig.h"
#include "BBBThrowableDefinition.generated.h"

/** 独立物理投掷物静态配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBThrowableDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "发射配置"))
    FBBBThrowableLaunchConfig Launch;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "物理配置"))
    FBBBThrowablePhysicsConfig Physics;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "爆炸配置"))
    FBBBThrowableExplosionConfig Explosion;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "投掷物", meta = (DisplayName = "表现配置"))
    FBBBThrowablePresentationConfig Presentation;
    /** @return 配置是否能够形成有效物理投掷物 */
    bool IsValid() const;
};
