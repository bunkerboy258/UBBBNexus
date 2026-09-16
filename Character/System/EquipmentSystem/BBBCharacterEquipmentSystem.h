#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Initialization/BBBCharacterDefaultEquipmentInitializer.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Processors/BBBCharacterEquipmentSelectionProcessor.h"

class FBBBCharacterInitializer;
class ABBBCharacterInstance;
class USkeletalMeshComponent;
struct FBBBCharacterEquipmentConfig;
struct FBBBCharacterEquipmentRuntimeData;

/** 角色装备容器、选择与动作的唯一逻辑系统 */
class ABBB_EVAC_API FBBBCharacterEquipmentSystem final
{
public:
    /** 角色结束时清理所有持有装备 */
    void Shutdown();

    /** 在首次角色仲裁前按本地控制关系创建出生装备 */
    void InitializeDefaultEquipment();

    /** 维护装备选择并将本帧命令转发至装备 API */
    void Update();

private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化装备系统固定依赖并建立出生装备
     * @param InCharacterMesh	角色骨骼网格
     * @param InEquipmentData	角色装备黑板
     * @param InCharacterAPI	角色能力接口
     * @param InEquipmentOuter	装备实例生命周期所有者
     * @param InEquipmentConfig	角色装备配置
     */
    void Initialize(
        USkeletalMeshComponent &InCharacterMesh,
        FBBBCharacterEquipmentRuntimeData &InEquipmentData,
        ABBBCharacterInstance &InCharacter,
        const FBBBCharacterEquipmentConfig &InEquipmentConfig);

    /** 角色装备黑板 */
    FBBBCharacterEquipmentRuntimeData *EquipmentData = nullptr;

    /** 角色骨骼网格 */
    USkeletalMeshComponent *CharacterMesh = nullptr;

    /** 装备实例生命周期所有者 */
    ABBBCharacterInstance *Character = nullptr;

    /** 右手装备挂接插槽 */
    FName RightHandWeaponSocketName = NAME_None;

    /** 默认装备初始化器 */
    FBBBCharacterDefaultEquipmentInitializer DefaultEquipmentInitializer;

    /** 首次角色更新时才按最终本地控制关系创建镜像装备 */
    bool bDefaultEquipmentInitialized = false;

    /** 默认装备配置 */
    const FBBBCharacterEquipmentConfig *DefaultEquipmentConfig = nullptr;

    /** 装备选择处理器 */
    FBBBCharacterEquipmentSelectionProcessor SelectionProcessor;

    /** 装备动作处理器 */
    FBBBCharacterEquipmentActionProcessor ActionProcessor;

};
