#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/Definitions/BBBWorldDropInputSlot.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Initialize/FBBBWorldDropInitializeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Claim/FBBBWorldDropClaimLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Complete/FBBBWorldDropCompleteLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/AuthorityFact/Snapshot/FBBBWorldDropSnapshotAuthorityFactPacket.h"
/** 世界掉落物的固定输入槽位 */
struct FBBBWorldDropInputState final
{
    /** Initialize 固定输入槽 */
    TBBBWorldDropInputSlot<FBBBWorldDropInitializeLocalControlPacket> Initialize;
    /** Claim 固定输入槽 */
    TBBBWorldDropInputSlot<FBBBWorldDropClaimLocalControlPacket> Claim;
    /** Complete 固定输入槽 */
    TBBBWorldDropInputSlot<FBBBWorldDropCompleteLocalControlPacket> Complete;
    /** Snapshot 固定输入槽 */
    TBBBWorldDropInputSlot<FBBBWorldDropSnapshotAuthorityFactPacket> Snapshot;
};
