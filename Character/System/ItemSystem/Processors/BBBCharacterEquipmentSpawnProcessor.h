
#pragma once
#include "CoreMinimal.h"
class USkeletalMeshComponent;
class FBBBCharacterExternalAPI;
struct FBBBCharacterItemRuntimeData;

class FBBBCharacterEquipmentSpawnProcessor final
{
public:

    void Update(
        USkeletalMeshComponent &CharacterMesh,
        float WorldTimeSeconds,
        FName RightHandWeaponSocketName,
        FBBBCharacterItemRuntimeData &ItemData,
        FBBBCharacterExternalAPI &CharacterAPI) const;
};
