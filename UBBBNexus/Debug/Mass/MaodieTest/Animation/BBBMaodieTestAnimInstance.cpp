#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Animation/BBBMaodieTestAnimInstance.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Animation/BBBMaodieTestAnimInstanceProxy.h"
FAnimInstanceProxy *UBBBMaodieTestAnimInstance::CreateAnimInstanceProxy()
{
    return new FBBBMaodieTestAnimInstanceProxy(this);
}
