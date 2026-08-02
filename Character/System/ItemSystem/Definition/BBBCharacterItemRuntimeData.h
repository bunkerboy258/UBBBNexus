
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemCommands.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemEvents.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBCharacterItemRuntimeData.generated.h"
class FBBBCharacterDefaultItemInitializer;
class FBBBCharacterEquipmentSpawnProcessor;
class FBBBCharacterInitializer;
class FBBBCharacterItemSystem;

USTRUCT(BlueprintType)
struct FBBBCharacterItemRuntimeData
{
    GENERATED_BODY()

    void CleanFrame()
    {
        Commands.CleanFrame();
        Events.CleanFrame();
    }
private:
    friend class FBBBCharacterDefaultItemInitializer;
    friend class FBBBCharacterEquipmentSpawnProcessor;
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterItemSystem;

    UPROPERTY()
    FBBBCharacterItemCommands Commands;

    UPROPERTY()
    FBBBCharacterItemState State;

    UPROPERTY()
    FBBBCharacterItemEvents Events;
};
