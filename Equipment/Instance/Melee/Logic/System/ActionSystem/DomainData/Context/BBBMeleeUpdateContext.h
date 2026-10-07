#pragma once
class ABBBMeleeEquipment;
class ABBBCharacter;
class USkeletalMeshComponent;
class UBBBMeleeDefinition;
class UWorld;
struct FBBBMeleeRuntimeData;
/** 近战系统本帧读取的装配依赖 */
struct FBBBMeleeUpdateContext final
{
    /** 本件装备 */
    ABBBMeleeEquipment &Equipment;
    /** 持有角色 */
    ABBBCharacter &Character;
    /** 当前武器姿势 */
    USkeletalMeshComponent &Mesh;
    /** 单件静态配置 */
    const UBBBMeleeDefinition &Definition;
    /** 唯一黑板 */
    FBBBMeleeRuntimeData &Data;
    /** 当前世界 */
    UWorld &World;
    /** 是否负责本机玩法因果 */
    bool bCausal = true;
};
