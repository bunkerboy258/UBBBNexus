#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"
class APawn;
class FBBBCharacterNetworkSystem;

//注册为受UE对象系统管理的类
UCLASS(ClassGroup = "BBB")
/*UE的RPC必须声明在UCLASS内部 故使用一个自定义组件承载角色控制器与引擎的桥接需求*/
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:

    /**
     * 构造网络组件 关闭Tick并默认启用网络复制
     */
    UBBBCharacterNetworkComponent();

    /**
     * 绑定角色网络逻辑系统
     * @param InNetworkSystem 角色网络逻辑系统
     */
    void Initialize(FBBBCharacterNetworkSystem &InNetworkSystem);

    /**
     * 登记需要参与网络复制的成员
     * @param OutLifetimeProps	输出的复制属性列表
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
    friend class FBBBCharacterNetworkSystem;

    /** @return 拥有者是否由本机控制 */
    bool IsOwnerLocallyControlled() const;

    /** @return 拥有者是否具备服务器权威 */
    bool IsOwnerAuthority() const;

    /**
     * 写入权威瞄准复制属性
     * @param AimState 权威瞄准状态
     */
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);

    UFUNCTION(Server, Reliable)
    void ServerUploadEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(Server, Reliable)
    void ServerUploadFirePacket(FBBBFireNetworkPacket Packet);

    UFUNCTION(Server, Reliable)
    void ServerUploadReloadPacket(FBBBReloadNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastFirePacket(FBBBFireNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastReloadPacket(FBBBReloadNetworkPacket Packet);

    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

    /**
     * 驱动模拟端还原瞄准状态
     */
    UFUNCTION()
    void OnRep_ReplicatedAimState();

    /**
     * 读取拥有者Pawn
     * @return 拥有者Pawn
     */
    APawn *GetOwnerPawn() const;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    //瞄准状态 自动从服务器同步到客户端
    FBBBAimNetworkState ReplicatedAimState;

    FBBBCharacterNetworkSystem *NetworkSystem = nullptr;
};
