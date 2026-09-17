#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
class USkeletalMeshComponent;
class FBBBCharacterExternalAPI;
class FBBBCharacterEquipmentLifecycleProcessor;

/** 在配置注入后装配装备组件与运行数据 */
class ABBB_EVAC_API FBBBEquipmentInitializer final
{
private:
    friend class FBBBCharacterEquipmentLifecycleProcessor;

    /** 校验配置并初始化固定系统 */
    static bool Initialize(ABBBEquipmentInstance &Instance,
        USkeletalMeshComponent &CharacterMesh, FBBBCharacterExternalAPI &CharacterAPI);
};
