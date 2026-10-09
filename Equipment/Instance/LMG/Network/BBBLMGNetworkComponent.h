#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "BBBLMGNetworkComponent.generated.h"

/** 轻机枪独占的协议与输入转换 */
UCLASS()
class ABBB_EVAC_API UBBBLMGNetworkComponent final : public UBBBEquipmentNetworkComponent
{
    GENERATED_BODY()

public:
    UBBBLMGNetworkComponent();

private:
    friend class FBBBLMGNetworkProcessor;

    /** @param Sequence	动作序号 @return 是否接受发送 */
    bool PublishFire(int32 Sequence);

    /** @param Sequence	动作序号 @return 是否接受发送 */
    bool PublishReloadStart(int32 Sequence);

    /** @param Sequence	动作序号 @param bCompleted 是否装填完成 @return 是否接受发送 */
    bool PublishReloadEnd(int32 Sequence, bool bCompleted);

    virtual bool ReceiveMessage(uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage) override;
};
