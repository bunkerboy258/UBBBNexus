#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Processors/BBBCharacterEquipmentSelectionProcessor.h"

class FBBBCharacterInitializer;
class FBBBCharacterUpdatePipeline;
class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentConfig;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色装备容器、选择与动作的唯一逻辑系统 */
class ABBB_EVAC_API FBBBCharacterEquipmentController final
{
public:
    /** @return 当前激活主手装备 */
    ABBBEquipment *GetActiveEquipment() const;

private:
    friend class FBBBCharacterInitializer;
    friend class FBBBCharacterUpdatePipeline;

    /** 维护装备选择并将本帧命令转发至装备 API */
    void Update();

    /**
     * 注入装备系统依赖并建立空容器
     * @param InCharacterMesh	角色骨骼网格
     * @param InEquipmentData	角色装备黑板
     * @param InCharacter	装备实例生命周期所有者
     * @param InEquipmentConfig	角色装备配置
     * @return 无
     */
    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        FBBBCharacterEquipmentRuntimeData &InEquipmentData,
        ABBBCharacter &InCharacter,
        const FBBBCharacterEquipmentConfig &InEquipmentConfig);

    /** 角色装备黑板 */
    FBBBCharacterEquipmentRuntimeData *EquipmentData = nullptr;

    /** 角色骨骼网格 */
    USkeletalMeshComponent *CharacterMesh = nullptr;

    /** 装备实例生命周期所有者 */
    ABBBCharacter *Character = nullptr;

    /** 右手装备挂接插槽 */
    FName RightHandWeaponSocketName = NAME_None;

    /** 装备选择处理器 */
    FBBBCharacterEquipmentSelectionProcessor SelectionProcessor;

    /** 装备动作处理器 */
    FBBBCharacterEquipmentActionProcessor ActionProcessor;

};
