
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
class FBBBCharacterExternalAPI;
class UBBBEquipmentCatalog;
struct FBBBCharacterItemRuntimeData;

class FBBBCharacterEquipmentSpawnProcessor final
{
public:

    void Update(
        USkeletalMeshComponent &CharacterMesh,
        float WorldTimeSeconds,
        FName RightHandWeaponSocketName,
        UBBBEquipmentCatalog &EquipmentCatalog,
        FBBBCharacterItemRuntimeData &ItemData,
        FBBBCharacterExternalAPI &CharacterAPI) const;
};
