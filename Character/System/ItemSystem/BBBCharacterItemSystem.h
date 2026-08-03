
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

    /**
     * 逐帧驱动物品系统 更新装备状态并执行物品动作
     */
    void Update();
private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化物品系统依赖 建立背包槽位并生成出生物品
     * @param InCharacterMesh	角色骨骼网格
     * @param InItemData	角色物品运行时数据
     * @param InCharacterAPI	角色能力接口
     * @param InItemOuter	物品实例生命周期所有者
     * @param InEquipmentConfig	角色装备配置
     * @param InItemConfig	角色物品配置
     * @param InWorldData	角色世界运行时数据
     */
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
