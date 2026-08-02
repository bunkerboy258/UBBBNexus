
#pragma once
#include "CoreMinimal.h"
//封装UBBB角色网络组件的数据与行为
class UBBBCharacterNetworkComponent;
struct FBBBCharacterEquipmentState;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBEquipmentUploadProcessor final
{
public:

    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};
