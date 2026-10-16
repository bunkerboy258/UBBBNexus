#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/BBBGrenadeNetworkSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/RemoteMessage/Use/FBBBGrenadeStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/AuthorityFact/Use/FBBBGrenadeStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeShowLocalControlPacket.h"
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
    /** @param Packet 本类输入 @return 无 仅贡献输入 */
    template<typename TPacket>
    void SubmitInput(TPacket &&Packet)
    {
        QueueInput(Forward<TPacket>(Packet));
    }
    virtual bool IsMeshHidden() const override;
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
    virtual void QueueInput(FBBBConsumableHideLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadeHideLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumableShowLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadeShowLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadePrimaryLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumableEquipLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadeEquipLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumableReleaseLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadeReleaseLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) override;
    void QueueInput(FBBBGrenadeFinishLocalControlPacket Packet);
    virtual void QueueInput(FBBBConsumableStateRemoteMessagePacket<> Packet) override;
    void QueueInput(FBBBGrenadeStateRemoteMessagePacket Packet);
    virtual void QueueInput(FBBBConsumableStateAuthorityFactPacket<> Packet) override;
    void QueueInput(FBBBGrenadeStateAuthorityFactPacket Packet);
    FBBBGrenadeParseSystem ParseSystem;
    FBBBGrenadeUseSystem UseSystem;
    FBBBGrenadeNetworkSystem NetworkSystem;
};
