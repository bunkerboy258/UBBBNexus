#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkMessage.h"
#include "BBBEquipmentNetworkComponent.generated.h"

class ABBBEquipment;

/** 公共消息载体与具体装备协议组件的基类 */
UCLASS(ClassGroup = "BBB")
class ABBB_EVAC_API UBBBEquipmentNetworkComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBBBEquipmentNetworkComponent();

    /** @param OutLifetimeProps	复制字段 @return 无 */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    /** @return 无 向已经创建的当前实例投递待接收消息 */
    void DeliverPending();

protected:
    /** @param Kind	本类协议编号 @param Data	已编码结果 @return 是否接受发送 */
    bool PublishMessage(uint8 Kind, TArray<uint8> Data);

    /** @param Kind	协议编号 @param Data	消息内容 @param Revision	消息顺序 @param bRemoteMessage	是否为主机收到的事件 @return 是否接受 */
    virtual bool ReceiveMessage(uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage);

private:
    /** @param Message	本机已经成立的结果 @return 无 */
    UFUNCTION(Server, Reliable)
    void ServerSubmitMessage(FBBBEquipmentNetworkMessage Message);

    /** @return 无 当前消息到达后尝试投递 */
    UFUNCTION()
    void OnRep_Messages();

    /** @param Message	待保存的消息 @param Destination	当前协议槽位 @return 无 */
    static void StoreMessage(FBBBEquipmentNetworkMessage Message, TArray<FBBBEquipmentNetworkMessage> &Destination);

    /** 每个协议编号仅保留当前消息 不保存事件历史 */
    UPROPERTY(ReplicatedUsing = OnRep_Messages)
    TArray<FBBBEquipmentNetworkMessage> Messages;

    /** 主机等待持有关系建立的当前事件 */
    TArray<FBBBEquipmentNetworkMessage> RemoteMessages;

    /** 当前发送实例 */
    uint64 PublishedGeneration = 0;

    /** 当前发送顺序 */
    uint64 PublishedRevision = 0;

    /** 当前接收实例 */
    uint64 DeliveredGeneration = 0;

    /** 当前接收使用许可 */
    uint64 DeliveredUseRevision = 0;

    /** 各协议槽位已经消费的顺序 */
    TMap<uint8, uint64> DeliveredRevisions;
};
