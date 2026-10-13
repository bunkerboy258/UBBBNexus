#pragma once
#include "CoreMinimal.h"
class ABBBCharacter;
class USkeletalMeshComponent;
class AActor;
struct FBBBCharacterRuntimeData;

/** 角色持有交互管线的一次调用数据 */
struct FBBBCharacterInteractionUpdateContext final
{
    ABBBCharacter &Character;
    USkeletalMeshComponent &CharacterMesh;
    FBBBCharacterRuntimeData &RuntimeData;
    FName RightHandSocketName = NAME_None;
    bool bIsMirror = true;
    AActor *DesiredItem = nullptr;
    TSubclassOf<AActor> PendingItemClass;
    bool bHasSelectionResult = false;
    uint64 DesiredGeneration = 0;
};
