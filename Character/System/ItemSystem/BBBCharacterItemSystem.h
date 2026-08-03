
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Initialization/BBBCharacterDefaultItemInitializer.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterEquipmentProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Processors/BBBCharacterItemActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Storage/BBBCharacterItemStorage.h"
class FBBBCharacterExternalAPI;
class FBBBCharacterInitializer;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentConfig;
struct FBBBCharacterItemConfig;
struct FBBBCharacterItemRuntimeData;
struct FBBBCharacterWorldRuntimeData;

class ABBB_EVAC_API FBBBCharacterItemSystem final
{
public:

    void Update();
private:
    friend class FBBBCharacterInitializer;

    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        FBBBCharacterItemRuntimeData &InItemData,
        FBBBCharacterExternalAPI &InCharacterAPI,
        UObject &InItemOuter,
        const FBBBCharacterEquipmentConfig &InEquipmentConfig,
        const FBBBCharacterItemConfig &InItemConfig,
        const FBBBCharacterWorldRuntimeData &InWorldData);

    FBBBCharacterItemRuntimeData *ItemData = nullptr;

    const FBBBCharacterWorldRuntimeData *WorldData = nullptr;

    FBBBCharacterExternalAPI *CharacterAPI = nullptr;

    USkeletalMeshComponent *CharacterMesh = nullptr;

    FName RightHandWeaponSocketName = NAME_None;

    FBBBCharacterItemStorage Storage;

    FBBBCharacterDefaultItemInitializer DefaultItemInitializer;

    FBBBCharacterEquipmentProcessor EquipmentProcessor;

    FBBBCharacterItemActionProcessor ItemActionProcessor;
};
