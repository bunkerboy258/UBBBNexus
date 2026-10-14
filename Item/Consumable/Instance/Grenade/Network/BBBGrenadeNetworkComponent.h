#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Network/BBBGrenadeNetworkMessage.h"
#include "BBBGrenadeNetworkComponent.generated.h"
/** 手雷专属协议组件 公共复制载体由角色持有 */
UCLASS(ClassGroup="BBB")
class ABBB_EVAC_API UBBBGrenadeNetworkComponent final : public UBBBConsumableNetworkComponent
{
    GENERATED_BODY()
public:
    UBBBGrenadeNetworkComponent();
    /** @param Message 已成立的当前使用结果 @return 是否成功提交载体 */
    bool PublishState(FBBBGrenadeNetworkMessage Message);
protected:
    /** @param Kind 协议编号 @param Data 编码数据 @param Revision 消息顺序 @param bRemoteMessage 主机接收 @return 是否接受 */
    virtual bool ReceiveMessage(uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage) override;
};
