#include "BBBWork/UBBBNexus/Equipment/Core/Config/Fire/BBBEquipmentFireConfig.h"

#include "BBBWork/UBBBNexus/Item/Projectile/BBBBulletActor.h"

FBBBEquipmentFireConfig::FBBBEquipmentFireConfig()
{
    BulletActorClass = ABBBBulletActor::StaticClass();
}
