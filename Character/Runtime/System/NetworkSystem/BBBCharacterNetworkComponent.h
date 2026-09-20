#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkFactLedgerState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/State/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class ABBBCharacter;
class FBBBCharacterNetworkSystem;

UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBCharacterNetworkComponent();

    void Initialize(ABBBCharacter &InCharacter);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBCharacterNetworkSystem;
    friend struct FBBBNetworkFactLedgerState;

    bool IsOwnerLocallyControlled() const;
    bool IsOwnerAuthority() const;

    void PublishEquipmentFact(FBBBEquipmentActionFact Fact);
    void PublishEquipmentState(FName EquipmentId);
    void PublishAimState(const FBBBAimNetworkState &AimState);
    void PublishLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    void DeliverEquipmentFact(const FBBBEquipmentActionFact &Fact);
    void DeliverEquipmentState(FName EquipmentId);
    void DeliverAimState(const FBBBAimNetworkState &AimState);
    void DeliverLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipmentFact(FBBBEquipmentActionFact Fact);

    UFUNCTION(Server, Reliable)
    void ServerSubmitEquipmentState(FName EquipmentId);

    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

    UFUNCTION(Server, Unreliable)
    void ServerSubmitLocomotionState(FBBBLocomotionNetworkState LocomotionState);

    UFUNCTION()
    void OnRep_ReplicatedEquipmentId();

    UFUNCTION()
    void OnRep_ReplicatedAimState();

    UFUNCTION()
    void OnRep_ReplicatedLocomotionState();

    APawn *GetOwnerPawn() const;

    UPROPERTY(Replicated)
    FBBBNetworkFactLedgerState ReplicatedFactLedger;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedEquipmentId)
    FName ReplicatedEquipmentId = NAME_None;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBAimNetworkState ReplicatedAimState;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
    FBBBLocomotionNetworkState ReplicatedLocomotionState;

    ABBBCharacter *Character = nullptr;
};
