#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;

/** 装备签名本次解析使用的栈上数据 */
struct FBBBEquipmentRuntimeContext final
{
    /** 当前装备演员 */
    ABBBEquipment &Equipment;

    /** 拥有当前装备的角色 */
    ABBBCharacter &Character;

    /** 当前装备的骨骼网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前帧间隔 */
    float DeltaSeconds = 0.0f;
};
