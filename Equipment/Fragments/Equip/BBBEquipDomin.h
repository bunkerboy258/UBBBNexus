#pragma once

#include "CoreMinimal.h"
#include "BBBEquipDomin.generated.h"

class ABBBEquipmentPresentationActor;
class FBBBCharacterExternalAPI;
class UBBBEquipRuntimeData;
class USkeletalMeshComponent;

/** 装备操作域基类 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipDomin
{
    GENERATED_BODY()

public:
    virtual ~FBBBEquipDomin() = default;

    /**
     * 创建装备操作域运行数据
     * @param Outer	运行数据所有者
     * @return 创建完成的运行数据
     */
    virtual UBBBEquipRuntimeData *InitializeRuntimeData(UObject &Outer) const;

    /**
     * 生成并挂接装备表现实体
     * @param RuntimeData			装备操作域运行数据
     * @param CharacterMesh		角色骨骼网格
     * @param CharacterAPI		角色能力接口
     * @param AttachmentSocketName	装备挂接插槽
     * @return 生成完成的表现实体
     */
    virtual ABBBEquipmentPresentationActor *Equip(
        UBBBEquipRuntimeData &RuntimeData,
        USkeletalMeshComponent &CharacterMesh,
        FBBBCharacterExternalAPI &CharacterAPI,
        FName AttachmentSocketName) const;

    /**
     * 更新装备操作域
     * @param CharacterAPI		角色能力接口
     * @param PresentationActor	装备表现实体
     * @param RuntimeData			装备操作域运行数据
     */
    virtual void Update(
        FBBBCharacterExternalAPI &CharacterAPI,
        ABBBEquipmentPresentationActor &PresentationActor,
        UBBBEquipRuntimeData &RuntimeData) const;

    /** @return 瞄准来源插槽 */
    virtual FName GetAimSourceSocketName() const;

    /** @return 左手握持插槽 */
    virtual FName GetLeftHandGripSocketName() const;

    /** @return 左手握持插槽修正 */
    virtual const FTransform &GetLeftHandGripSocketLocalOffset() const;

    /** @return 是否启用左手逆向动力学 */
    virtual bool IsLeftHandIKEnabled() const;
};
