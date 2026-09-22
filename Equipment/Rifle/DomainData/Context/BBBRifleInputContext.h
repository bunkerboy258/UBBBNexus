#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBRifleEquipment;
class UBBBRifleDefinition;
class USkeletalMeshComponent;
class UWorld;
struct FBBBRifleRuntimeData;

/** 步枪输入解析期间使用的单帧上下文 */
struct FBBBRifleInputContext final
{
    /** 当前步枪演员 */
    ABBBRifleEquipment &Equipment;

    /** 当前装备持有角色 */
    ABBBCharacter &Character;

    /** 当前步枪骨骼网格 */
    USkeletalMeshComponent &WeaponMesh;

    /** 当前步枪静态配置 */
    const UBBBRifleDefinition &Definition;

    /** 当前步枪运行时事实 */
    FBBBRifleRuntimeData &RuntimeData;

    /** 当前帧间隔 */
    float DeltaSeconds = 0.0f;

    /** 本次解析所属世界 */
    UWorld *World = nullptr;
};
