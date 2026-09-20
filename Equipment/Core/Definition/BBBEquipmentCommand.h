#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBEquipmentCommand.generated.h"

/** 角色跨入装备领域的统一命令种类 */
UENUM()
enum class EBBBEquipmentCommandType : uint8
{
    Equip,
    Fire,
    Reload,
    DetachMagazine,
    LoadMagazine,
    CancelReload,
    Fact
};

/** 角色向当前装备提交的通用命令信封 */
USTRUCT()
struct ABBB_EVAC_API FBBBEquipmentCommand
{
    GENERATED_BODY()

    EBBBEquipmentCommandType Type = EBBBEquipmentCommandType::Fire;

    int32 Sequence = INDEX_NONE;

    FBBBEquipmentActionFact Fact;
};
