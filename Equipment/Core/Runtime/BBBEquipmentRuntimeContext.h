#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;

/** 武器签名解析期间可读取的外部运行时上下文 */
struct FBBBEquipmentRuntimeContext
{
    ABBBEquipment &Equipment;

    ABBBCharacter &Character;

    USkeletalMeshComponent &WeaponMesh;

    float DeltaSeconds = 0.0f;

    bool bIsMirror = false;
};
