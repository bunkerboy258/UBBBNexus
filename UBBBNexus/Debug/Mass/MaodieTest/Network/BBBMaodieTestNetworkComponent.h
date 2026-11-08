#pragma once
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBMaodieTestNetworkComponent.generated.h"
/** 耄耋当前表现事实的复制桥接 */
UCLASS()
class UBBBMaodieTestNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:
    UBBBMaodieTestNetworkComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    /** @param Value 当前事实 @return 无 */
    void Publish(const FBBBMaodieTestSnapshot &Value);
private:
    UPROPERTY(ReplicatedUsing = OnRep_Snapshot)
    FBBBMaodieTestSnapshot Snapshot;
    UFUNCTION()
    void OnRep_Snapshot();
};
