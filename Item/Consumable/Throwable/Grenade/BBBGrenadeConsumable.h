#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/RuntimeData/BBBGrenadeRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/BBBGrenadeParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/UseSystem/BBBGrenadeUseSystem.h"
#include "BBBGrenadeConsumable.generated.h"
/** 本次Grenade使用的轻量运行时 */
UCLASS()
class ABBB_EVAC_API UBBBGrenadeConsumable final : public UBBBConsumable
{
    GENERATED_BODY()
  public:
    virtual bool IsUsing() const override;
    /** 当次使用的唯一运行时黑板 */
    FBBBGrenadeRuntimeData RuntimeData;

  private:
    friend class FBBBGrenadeInitializer;
    friend class FBBBGrenadeUpdatePipeline;
    friend class FBBBGrenadeShutdown;
    virtual bool InitializeRuntimeData() override;
    virtual void UpdateRuntimeData() override;
    virtual void ShutdownRuntimeData() override;
    virtual void QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) override;
    FBBBGrenadeParseSystem ParseSystem;
    FBBBGrenadeUseSystem UseSystem;
};
