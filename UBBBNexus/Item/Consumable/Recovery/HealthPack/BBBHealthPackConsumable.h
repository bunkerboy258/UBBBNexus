#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/RuntimeData/BBBHealthPackRuntimeData.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/BBBHealthPackParseSystem.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/UseSystem/BBBHealthPackUseSystem.h"
#include "BBBHealthPackConsumable.generated.h"
/** 本次HealthPack使用的轻量运行时 */
UCLASS()
class ABBB_EVAC_API UBBBHealthPackConsumable final : public UBBBConsumable
{
    GENERATED_BODY()
  public:
    virtual bool IsUsing() const override;
    /** 当次使用的唯一运行时黑板 */
    FBBBHealthPackRuntimeData RuntimeData;

  private:
    friend class FBBBHealthPackInitializer;
    friend class FBBBHealthPackUpdatePipeline;
    friend class FBBBHealthPackShutdown;
    virtual bool InitializeRuntimeData() override;
    virtual void UpdateRuntimeData() override;
    virtual void ShutdownRuntimeData() override;
    virtual void QueueInput(FBBBConsumablePrimaryLocalControlPacket<> Packet) override;
    virtual void QueueInput(FBBBConsumableFinishLocalControlPacket<> Packet) override;
    FBBBHealthPackParseSystem ParseSystem;
    FBBBHealthPackUseSystem UseSystem;
};
