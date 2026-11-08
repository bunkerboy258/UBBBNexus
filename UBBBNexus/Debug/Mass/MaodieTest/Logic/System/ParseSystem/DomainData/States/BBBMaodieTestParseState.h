#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/DomainData/Definitions/BBBMaodieTestSnapshot.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Input/LocalControl/Behavior/FBBBMaodieTestProjectileHitLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Input/RemoteMessage/Behavior/FBBBMaodieTestProjectileHitRemoteMessagePacket.h"
#include "BBBMaodieTestParseState.generated.h"
class APawn;
/** 耄耋Parse状态 */
USTRUCT()
struct FBBBMaodieTestParseState final
{
    GENERATED_BODY()
    /** 待解析的最新事实 */
    UPROPERTY()
    FBBBMaodieTestSnapshot Pending;
    /** 是否有待解析输入 */
    bool bActive = false;
    /** 本机射手命中输入 */
    FBBBMaodieTestProjectileHitLocalControlPacket LocalHits;
    /** 本机命中槽是否有输入 */
    bool bLocalHitsActive = false;
    /** 远端射手命中输入 */
    FBBBMaodieTestProjectileHitRemoteMessagePacket RemoteHits;
    /** 远端命中槽是否有输入 */
    bool bRemoteHitsActive = false;
};
