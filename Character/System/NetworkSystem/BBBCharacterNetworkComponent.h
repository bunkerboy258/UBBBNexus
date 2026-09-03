#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class FBBBCharacterNetworkSystem;

/** 承载角色状态与动作RPC的引擎组件 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBCharacterNetworkComponent();

    /** @param InNetworkSystem 角色网络逻辑系统 */
    void Initialize(FBBBCharacterNetworkSystem &InNetworkSystem);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBCharacterNetworkSystem;

    /** @return 拥有者是否由本机控制 */
    bool IsOwnerLocallyControlled() const;

    /** @return 拥有者是否具备服务器权威 */
    bool IsOwnerAuthority() const;

    /** @param AimState 权威瞄准状态 */
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);

    /** @param LocomotionState 权威移动状态 */
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

    /** @return 拥有者角色 */
    APawn *GetOwnerPawn() const;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBAimNetworkState ReplicatedAimState;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
    FBBBLocomotionNetworkState ReplicatedLocomotionState;

    FBBBCharacterNetworkSystem *NetworkSystem = nullptr;
};
