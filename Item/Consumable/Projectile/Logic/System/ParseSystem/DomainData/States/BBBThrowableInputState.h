#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Input/LocalControl/Lifecycle/FBBBThrowableLaunchLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Input/LocalControl/Explosion/FBBBThrowableDetonateLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Input/AuthorityFact/Explosion/FBBBThrowableExplosionAuthorityFactPacket.h"
/** 每种输入的独立覆盖槽位 */
struct FBBBThrowableInputState final
{
    /** 释放槽位 */
    bool bLaunch = false;
    FBBBThrowableLaunchLocalControlPacket Launch;
    /** 接触引爆槽位 */
    bool bDetonate = false;
    FBBBThrowableDetonateLocalControlPacket Detonate;
    /** 终态事实槽位 */
    bool bFact = false;
    FBBBThrowableExplosionAuthorityFactPacket Fact;
};
