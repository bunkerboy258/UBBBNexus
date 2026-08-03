#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/States/BBBNetworkStates.h"
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

    /**
     * 构造网络组件 关闭Tick并默认启用网络复制
     */
    UBBBCharacterNetworkComponent();

    /**
     * 绑定网络运行时数据
     * @param InNetworkData	网络运行时数据
     */
    void Initialize(FBBBNetworkRuntimeData &InNetworkData);

    /**
     * 登记需要参与网络复制的成员
     * @param OutLifetimeProps	输出的复制属性列表
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    /**
     * 写入权威瞄准状态 并驱动服务器本地还原
     * @param AimState	权威瞄准状态
     */
    void SetReplicatedAimState(const FBBBAimNetworkState &AimState);

    /**
     * 上传或直接校验装备请求
     * @param Packet	装备网络Packet
     */
    void SendEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    /**
     * 上传或广播射击事件
     * @param Packet	开火网络Packet
     */
    void SendFirePacket(FBBBFireNetworkPacket Packet);

    /**
     * 上传或广播换弹事件
     * @param Packet	换弹网络Packet
     */
    void SendReloadPacket(FBBBReloadNetworkPacket Packet);

    /**
     * 提交瞄准同步状态
     * @param AimState	瞄准同步状态
     */
    void SendAimState(FBBBAimNetworkState AimState);

    /**
     * 将装备请求发送到服务器 可靠
     * @param Packet	装备网络Packet
     */
    UFUNCTION(Server, Reliable)
    void ServerUploadEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    /**
     * 将射击事件发送到服务器 可靠
     * @param Packet	开火网络Packet
     */
    UFUNCTION(Server, Reliable)
    void ServerUploadFirePacket(FBBBFireNetworkPacket Packet);

    /**
     * 将换弹事件发送到服务器 可靠
     * @param Packet	换弹网络Packet
     */
    UFUNCTION(Server, Reliable)
    void ServerUploadReloadPacket(FBBBReloadNetworkPacket Packet);

    /**
     * 向所有连接广播装备数据包 可靠
     * @param Packet	装备网络Packet
     */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastEquipmentPacket(FBBBEquipmentNetworkPacket Packet);

    /**
     * 将确认后的射击事件广播给所有客户端 可靠
     * @param Packet	开火网络Packet
     */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastFirePacket(FBBBFireNetworkPacket Packet);

    /**
     * 将确认后的换弹事件广播给所有客户端 可靠
     * @param Packet	换弹网络Packet
     */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastReloadPacket(FBBBReloadNetworkPacket Packet);

    /**
     * 将瞄准状态发送到服务器 不可靠
     * @param AimState	瞄准同步状态
     */
    UFUNCTION(Server, Unreliable)
    void ServerSubmitAimState(FBBBAimNetworkState AimState);

private:

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

    FBBBNetworkRuntimeData *NetworkData = nullptr;
};
