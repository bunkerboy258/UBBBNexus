#pragma once

#include "CoreMinimal.h"

class ABBBCharacter;
class ABBBEquipment;
class USkeletalMeshComponent;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterShutdown;

/** 由角色集中管理持有装备的外层生命周期 */
class FBBBCharacterEquipmentLifecycleProcessor final
{
private:
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterShutdown;

    /** 创建并初始化角色持有的装备 */
    static ABBBEquipment *Create(ABBBCharacter &Character,
        TSubclassOf<ABBBEquipment> EquipmentClass, bool bIsMirror);

    /** 挂接装备并绑定角色动画 */
    static bool Attach(USkeletalMeshComponent &CharacterMesh,
        FName AttachmentSocketName, ABBBEquipment &Equipment);

    /** 解除角色持有关系并取消尚未完成的操作 */
    static void Detach(USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment);

    /** 移除依赖并销毁角色持有的装备 */
    static void Destroy(USkeletalMeshComponent *CharacterMesh, ABBBEquipment &Equipment);
};
