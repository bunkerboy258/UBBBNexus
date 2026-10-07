#pragma once

#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "BBBMeleeNetworkComponent.generated.h"

/** 近战独占的协议与输入转换 */
UCLASS()
class ABBB_EVAC_API UBBBMeleeNetworkComponent final : public UBBBEquipmentNetworkComponent
{
    GENERATED_BODY()

public:
    UBBBMeleeNetworkComponent();

private:
    friend class FBBBMeleeNetworkProcessor;

    /** @param Sequence	动作序号 @return 是否接受发送 */
    bool PublishAttackStart(int32 Sequence);

    /** @param Sequence	动作序号 @return 是否接受发送 */
    bool PublishAttackEnd(int32 Sequence);

    virtual bool ReceiveMessage(uint8 Kind, const TArray<uint8> &Data, uint64 Revision, bool bRemoteMessage) override;
};
