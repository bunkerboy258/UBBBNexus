
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/BBBItemInstance.h"
#include "BBBInventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EBBBInventoryContainerType : uint8
{

    Backpack UMETA(DisplayName = "Backpack"),

    Hotbar UMETA(DisplayName = "Hotbar")
};

USTRUCT(BlueprintType)
struct FBBBInventorySlotRef
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EBBBInventoryContainerType ContainerType = EBBBInventoryContainerType::Backpack;

    UPROPERTY(BlueprintReadOnly)
    int32 SlotIndex = INDEX_NONE;

    bool IsValid() const
    {
        return SlotIndex >= 0;
    }
};

USTRUCT(BlueprintType)
struct FBBBInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 SlotIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    FBBBItemInstance ItemInstance;
};

USTRUCT(BlueprintType)
struct FBBBInventoryList
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBBBInventoryEntry> Entries;

    int32 GetNumOccupiedSlots() const
    {
        return Entries.Num();
    }
};
