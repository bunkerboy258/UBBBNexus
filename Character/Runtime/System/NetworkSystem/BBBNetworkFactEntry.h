#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBNetworkFactEntry.generated.h"

USTRUCT()
/** 网络事实账本的单条增量定义 只承载已经成立的领域事实 */
struct FBBBNetworkFactEntry final : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    /** 由接收边界翻译为领域输入包的原始事实 */
    FBBBEquipmentActionFact Fact;
};
