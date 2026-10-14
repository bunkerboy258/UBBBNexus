#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/BBBGrenadeNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/RemoteMessage/Use/FBBBGrenadeStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadePermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/AuthorityFact/Use/FBBBGrenadeStateAuthorityFactPacket.h"
#include "BBBGrenadeConsumable.generated.h"
class FBBBGrenadeInitializer;
class FBBBGrenadeUpdatePipeline;
class FBBBGrenadeShutdown;

/** 破片手雷的独立使用实例 */
UCLASS(BlueprintType)
class ABBB_EVAC_API ABBBGrenadeConsumable final : public ABBBConsumable
{
    GENERATED_BODY()
public:
    /** 创建本类协议组件 */
    ABBBGrenadeConsumable();
    /** @param Packet 本类输入 @return 是否接受 */
    template<typename TPacket>
    bool SubmitInput(TPacket &&Packet)
    {
        return IsInGameThread() && QueueInput(Forward<TPacket>(Packet));
    }
    virtual bool IsUsing() const override;
    virtual bool IsReleased() const override;
    virtual int32 GetUseSerial() const override;
    /** 本实例唯一运行时黑板 */
    FBBBGrenadeRuntimeData RuntimeData;

private:
    friend class FBBBGrenadeInitializer;
    friend class FBBBGrenadeUpdatePipeline;
    friend class FBBBGrenadeShutdown;
    virtual bool InitializeRuntimeData() override;
    virtual void UpdateRuntimeData() override;
    virtual void ShutdownRuntimeData() override;
    virtual bool QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBGrenadePrimaryLocalControlPacket Packet);
    virtual bool QueueInput(FBBBConsumableEquipLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBGrenadeEquipLocalControlPacket Packet);
    virtual bool QueueInput(FBBBConsumableReleaseLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBGrenadeReleaseLocalControlPacket Packet);
    virtual bool QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBGrenadeFinishLocalControlPacket Packet);
    virtual bool QueueInput(FBBBConsumablePermissionLocalControlPacket<> Packet) override;
    bool QueueInput(FBBBGrenadePermissionLocalControlPacket Packet);
    virtual bool QueueInput(FBBBConsumableStateRemoteMessagePacket<> Packet) override;
    bool QueueInput(FBBBGrenadeStateRemoteMessagePacket Packet);
    virtual bool QueueInput(FBBBConsumableStateAuthorityFactPacket<> Packet) override;
    bool QueueInput(FBBBGrenadeStateAuthorityFactPacket Packet);
    FBBBGrenadeParseSystem ParseSystem;
    FBBBGrenadeUseSystem UseSystem;
    FBBBGrenadeNetworkSystem NetworkSystem;
};
