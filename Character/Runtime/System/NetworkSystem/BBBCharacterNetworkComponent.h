#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBEquipmentActionNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBCharacterCommandNetworkPackets.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"

class APawn;
class ABBBCharacter;
class FBBBCharacterNetworkSystem;
class FBBBCharacterNetworkCommandProcessor;

/** 角色网络传输组件 只收发同步数据 */
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
    friend class FBBBCharacterNetworkCommandProcessor;

    bool IsOwnerLocallyControlled() const;
    bool IsOwnerAuthority() const;
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);
    void SetReplicatedLocomotionState(const FBBBLocomotionNetworkState &LocomotionState);

    /** 将本机客户端连续控制还原为权威角色输入，允许后续帧覆盖过期数据 */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitControlPacket(FBBBCharacterControlNetworkPacket Packet);

    /** 将本机客户端离散动作可靠还原为权威角色输入 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitActionPacket(FBBBCharacterActionNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentActionPacket(FBBBEquipmentActionNetworkPacket Packet);

    UFUNCTION()
    void OnRep_ReplicatedAimState();

    UFUNCTION()
    void OnRep_ReplicatedLocomotionState();

    APawn *GetOwnerPawn() const;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    FBBBAimNetworkState ReplicatedAimState;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedLocomotionState)
    FBBBLocomotionNetworkState ReplicatedLocomotionState;

    ABBBCharacter *Character = nullptr;
};
