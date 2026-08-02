#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBAimNetworkState.h"
#include "Components/ActorComponent.h"
#include "BBBCharacterNetworkComponent.generated.h"
class APawn;
struct FBBBNetworkRuntimeData;

//注册为受UE对象系统管理的类
UCLASS(ClassGroup = "BBB")
/*UE的RPC必须声明在UCLASS内部 故使用一个自定义组件承载角色控制器与引擎的桥接需求*/
class ABBB_EVAC_API UBBBCharacterNetworkComponent final : public UActorComponent
{
    GENERATED_BODY()
public:

    UBBBCharacterNetworkComponent();

    void Initialize(FBBBNetworkRuntimeData &InNetworkData);

    //登记需要参与网络复制的成员
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    //写入权威瞄准状态 并驱动服务器本地还原
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);

    //上传或直接校验装备请求
    void SendEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    //上传或广播射击事件
    void SendFirePacket(FBBBFireNetworkPacket Packet);

    //上传或广播换弹事件
    void SendReloadPacket(FBBBReloadNetworkPacket Packet);

    //提交瞄准同步状态
    void SendAimState(FBBBAimNetworkState AimState);

    UFUNCTION(Server, Reliable)
    //将装备请求发送到服务器 可靠
    void ServerUploadEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(Server, Reliable)
    //将射击事件发送到服务器 可靠
    void ServerUploadFirePacket(FBBBFireNetworkPacket Packet);

    UFUNCTION(Server, Reliable)
    //将换弹事件发送到服务器 可靠
    void ServerUploadReloadPacket(FBBBReloadNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    //向所有连接广播装备数据包 可靠
    void MulticastEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    //将确认后的射击事件广播给所有客户端 可靠
    void MulticastFirePacket(FBBBFireNetworkPacket Packet);

    UFUNCTION(NetMulticast, Reliable)
    //将确认后的换弹事件广播给所有客户端 可靠
    void MulticastReloadPacket(FBBBReloadNetworkPacket Packet);

    UFUNCTION(Server, Unreliable)
    //将瞄准状态发送到服务器 不可靠
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

private:

    UFUNCTION()
    //驱动模拟端还原瞄准状态
    void OnRep_ReplicatedAimState();

    APawn *GetOwnerPawn() const;

    UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAimState)
    //瞄准状态 自动从服务器同步到客户端
    FBBBAimNetworkState ReplicatedAimState;

    FBBBNetworkRuntimeData *NetworkData = nullptr;
};
