#pragma once

#include "UObject/StrongObjectPtr.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Input/LocalControl/Spawn/FBBBProjectileSpawnLocalControlPacket.h"

class UMassEntityConfigAsset;

/** 等待世界帧尾创建的完整弹丸出生批次 */
struct FBBBMassProjectileSpawnBatch final
{
    TStrongObjectPtr<const UMassEntityConfigAsset> Config;
    TArray<FBBBProjectileSpawnLocalControlPacket> Packets;
};
