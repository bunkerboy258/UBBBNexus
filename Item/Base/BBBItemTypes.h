
#pragma once
#include "CoreMinimal.h"
#include "BBBItemTypes.generated.h"

UENUM(BlueprintType)
enum class EBBBEquipmentSlot : uint8
{
    MainHand UMETA(DisplayName = "Main Hand")
};

UENUM(BlueprintType)
enum class EBBBWeaponAnimType : uint8
{
    None UMETA(DisplayName = "None"),
    Rifle UMETA(DisplayName = "Rifle")
};
