#pragma once

#include "CoreMinimal.h"

class UBBBEquipmentCatalog;
struct FBBBCharacterDiscreteInput;
struct FBBBCharacterParseState;
struct FBBBEquipmentActionEvent;
struct FBBBCharacterRuntimeData;

/** 将外部输入包按固定优先级写入角色黑板 */
class FBBBCharacterInputProcessor final
{
public:
    void Update(FBBBCharacterRuntimeData &Data, UBBBEquipmentCatalog &Catalog, bool bRestoreMode) const;

private:
    static void CancelReload(FBBBCharacterParseState &State);
    static void ApplyEquipmentEvent(const FBBBEquipmentActionEvent &Event, FBBBCharacterRuntimeData &Data);
    static void ApplyReloadPhase(const FBBBCharacterDiscreteInput &Input, FBBBCharacterRuntimeData &Data);
    static void PublishControl(FBBBCharacterRuntimeData &Data);
};
