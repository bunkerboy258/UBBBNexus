#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class FBBBCharacterInput;
class FBBBCharacterNetworkSystem;

/** 角色网络传输组件 只收发同步数据 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBCharacterNetworkComponent();

    void Initialize(FBBBCharacterInput &InInput);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBCharacterNetworkSystem;

    bool IsOwnerLocallyControlled() const;
    bool IsOwnerAuthority() const;
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);
    void SetReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    UFUNCTION(Server, Reliable)
    void ServerUploadEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(Server, Reliable)
    void ServerUploadEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet);

    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

    UFUNCTION(Server, Reliable)
    void ServerSubmitLocomotionState(FBBBLocomotionNetworkState LocomotionState);

    UFUNCTION()
    void OnRep_ReplicatedAimState();

    UFUNCTION()
    void OnRep_ReplicatedLocomotionState();

    APawn *GetOwnerPawn() const;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBAimNetworkState ReplicatedAimState;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
    FBBBLocomotionNetworkState ReplicatedLocomotionState;

    FBBBCharacterInput *Input = nullptr;
};
