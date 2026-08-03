
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBItemDefinition.generated.h"
class UBBBItemFragment;
class UTexture2D;

//装备的握持类型
UENUM(BlueprintType)
enum class EBBBEquipmentSlot : uint8
{
    MainHand UMETA(DisplayName = "Main Hand")
};

//武器的动画类型
UENUM(BlueprintType)
enum class EBBBWeaponAnimType : uint8
{
    None UMETA(DisplayName = "None"),
    Rifle UMETA(DisplayName = "Rifle")
};

//库存位置
UENUM(BlueprintType)
enum class EBBBInventoryContainerType : uint8
{

    Backpack UMETA(DisplayName = "Backpack"),

    Hotbar UMETA(DisplayName = "Hotbar")
};

//
USTRUCT(BlueprintType)
struct FBBBInventorySlotRef
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EBBBInventoryContainerType ContainerType = EBBBInventoryContainerType::Backpack;

    UPROPERTY(BlueprintReadOnly)
    int32 SlotIndex = INDEX_NONE;

    bool IsValid() const
    { return SlotIndex >= 0; }
};

USTRUCT(BlueprintType)
struct FBBBInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 SlotIndex = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UBBBItemInstance> ItemInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FBBBInventoryList
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FBBBInventoryEntry> Entries;

    int32 GetNumOccupiedSlots() const
    { return Entries.Num(); }
};

UCLASS(Abstract, BlueprintType)
class ABBB_EVAC_API UBBBItemDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FName ItemId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|Item", meta = (ClampMin = "1"))
    int32 MaxStack = 1;

    UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "BBB|Item")
    TArray<TObjectPtr<UBBBItemFragment>> Fragments;

    template <typename T>
    const T *FindFragment() const
    {
        for (const UBBBItemFragment *Fragment : Fragments)
        {
            if (const T *TypedFragment = Cast<T>(Fragment))
            {
                return TypedFragment;
            }
        }
        return nullptr;
    }
};
