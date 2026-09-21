#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/AimController/DomainData/BBBAimDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/EquipmentController/DomainData/BBBCharacterEquipmentDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/Controller/LocomotionController/DomainData/BBBCharacterLocomotionDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/BBBCharacterExternalDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/BBBCharacterAnimationDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/BBBCharacterNetworkDomainState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/BBBCharacterParseDomainState.h"
#include "BBBCharacterRuntimeData.generated.h"

/** 角色全部领域状态持有者的唯一聚合黑板 */
USTRUCT()
struct FBBBCharacterRuntimeData final
{
    GENERATED_BODY()

    /** 角色外部世界与网络身份状态 */
    UPROPERTY(Transient)
    FBBBCharacterExternalDomainState External;

    /** 角色输入解析状态 */
    UPROPERTY(Transient)
    FBBBCharacterParseDomainState Parse;

    /** 角色瞄准状态 */
    UPROPERTY(Transient)
    FBBBAimDomainState Aim;

    /** 角色移动状态 */
    UPROPERTY(Transient)
    FBBBCharacterLocomotionDomainState Locomotion;

    /** 角色装备状态 */
    UPROPERTY(Transient)
    FBBBCharacterEquipmentDomainState Equipment;

    /** 角色动画状态 */
    UPROPERTY(Transient)
    FBBBCharacterAnimationDomainState Animation;

    /** 角色网络观察状态 */
    UPROPERTY(Transient)
    FBBBCharacterNetworkDomainState Network;
};
