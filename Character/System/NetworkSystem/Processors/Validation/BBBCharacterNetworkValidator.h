
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/Processors/BBBEquipmentValidationProcessor.h"
class UBBBCharacterNetworkComponent;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkValidator final
{
public:

    /**
     * 逐帧校验待校验装备Packet并广播通过者
     * @param NetworkData	网络运行时数据
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
private:
    FBBBEquipmentValidationProcessor EquipmentValidationProcessor;
};
