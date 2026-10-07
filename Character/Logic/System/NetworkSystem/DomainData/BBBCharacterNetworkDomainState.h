#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBEquipmentNetworkObservationState.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterLifeNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterDamageNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterDamageInboxState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBAimNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBRunNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBTraversalNetworkObservationState.h"
#include "BBBCharacterNetworkDomainState.generated.h"

class FBBBAimObservationProcessor;
class FBBBRunObservationProcessor;
class FBBBCharacterNetworkSystem;

/** 角色网络领域全部观察状态的唯一直接持有者 */
USTRUCT()
struct FBBBCharacterNetworkDomainState final
{
    GENERATED_BODY()

public:
    /** @return 生命结果发送基准 */
    const FBBBCharacterLifeNetworkObservationState &ReadLifeNetworkObservationState() const { return LifeObservationState; }
    /** @return 独立伤害转送基准 */
    const FBBBCharacterDamageNetworkObservationState &ReadDamageNetworkObservationState() const { return DamageObservationState; }
    /** @return 待投送的独立命中 */
    const FBBBCharacterDamageInboxState &ReadDamageInboxState() const { return DamageInboxState; }
    /** @return 瞄准网络观察状态 */
    const FBBBAimNetworkObservationState &ReadAimNetworkObservationState() const
    {
        return AimObservationState;
    }

    /** @return 跑步网络观察状态 */
    const FBBBRunNetworkObservationState &ReadRunNetworkObservationState() const
    {
        return RunObservationState;
    }

    /** @return 翻越网络观察基准 */
    const FBBBTraversalNetworkObservationState &ReadTraversalNetworkObservationState() const
    {
        return TraversalObservationState;
    }

    /** @return 实际持有关系的网络发送基准 */
    const FBBBEquipmentNetworkObservationState &ReadEquipmentNetworkObservationState() const
    {
        return EquipmentObservationState;
    }

private:
    friend class FBBBCharacterLifeObservationProcessor;
    friend class FBBBCharacterDamageObservationProcessor;
    friend class FBBBCharacterParseSystem;
    /** 生命结果发送基准 */
    FBBBCharacterLifeNetworkObservationState LifeObservationState;
    /** 伤害转送基准 */
    FBBBCharacterDamageNetworkObservationState DamageObservationState;
    /** 待投送的独立命中 */
    FBBBCharacterDamageInboxState DamageInboxState;
    friend class FBBBEquipmentObservationProcessor;
    FBBBEquipmentNetworkObservationState EquipmentObservationState;

    friend class FBBBTraversalObservationProcessor;
    /** 翻越独立离散同步生命周期 */
    UPROPERTY(Transient)
    FBBBTraversalNetworkObservationState TraversalObservationState;
    friend class FBBBAimObservationProcessor;
    friend class FBBBRunObservationProcessor;
    friend class FBBBCharacterNetworkSystem;

    /** 瞄准网络观察状态 */
    UPROPERTY(Transient)
    FBBBAimNetworkObservationState AimObservationState;

    /** 跑步网络观察状态 */
    UPROPERTY(Transient)
    FBBBRunNetworkObservationState RunObservationState;
};
