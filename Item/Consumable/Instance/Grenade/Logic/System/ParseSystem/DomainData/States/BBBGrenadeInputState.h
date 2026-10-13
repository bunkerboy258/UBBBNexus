#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumableFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/LocalControl/Use/FBBBConsumablePermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/RemoteMessage/Use/FBBBConsumableStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/AuthorityFact/Use/FBBBConsumableStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/Definitions/BBBGrenadeInputSlot.h"

/** 手雷Parse领域持久状态 */
struct FBBBGrenadeInputState final
{
    /** PrimaryLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumablePrimaryLocalControlPacket> PrimaryLocalControl;
    /** EquipLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumableEquipLocalControlPacket> EquipLocalControl;
    /** ReleaseLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumableReleaseLocalControlPacket> ReleaseLocalControl;
    /** FinishLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumableFinishLocalControlPacket> FinishLocalControl;
    /** PermissionLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumablePermissionLocalControlPacket> PermissionLocalControl;
    /** StateRemoteMessage 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumableStateRemoteMessagePacket> StateRemoteMessage;
    /** StateAuthorityFact 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBConsumableStateAuthorityFactPacket> StateAuthorityFact;
};
