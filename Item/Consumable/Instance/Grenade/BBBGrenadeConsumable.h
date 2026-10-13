#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/NetworkSystem/BBBGrenadeNetworkSystem.h"
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
    virtual bool IsUsing() const override;
    virtual bool IsReleased() const override;
    virtual int32 GetUseSerial() const override;
    /** @return 本实例的公开只读领域数据 */
    const FBBBGrenadeRuntimeData &ReadRuntimeData() const;

private:
    friend class FBBBGrenadeInitializer;
    friend class FBBBGrenadeUpdatePipeline;
    friend class FBBBGrenadeShutdown;
    virtual bool InitializeRuntimeData() override;
    virtual void UpdateRuntimeData() override;
    virtual void ShutdownRuntimeData() override;
    virtual bool QueueInput(FBBBConsumablePrimaryLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBConsumableEquipLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBConsumableReleaseLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBConsumableFinishLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBConsumablePermissionLocalControlPacket Packet) override;
    virtual bool QueueInput(FBBBConsumableStateRemoteMessagePacket Packet) override;
    virtual bool QueueInput(FBBBConsumableStateAuthorityFactPacket Packet) override;
    FBBBGrenadeRuntimeData RuntimeData;
    FBBBGrenadeParseSystem ParseSystem;
    FBBBGrenadeUseSystem UseSystem;
    FBBBGrenadeNetworkSystem NetworkSystem;
};
