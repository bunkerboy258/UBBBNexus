
#pragma once
#include "CoreMinimal.h"
#include "BBBEquipmentNetworkPacket.generated.h"


//注册到UE反射与序列化系统
USTRUCT()
struct FBBBEquipmentNetworkPacket
{
    //UE 反射系统的必备宏
    GENERATED_BODY()

    UPROPERTY()
    FName EquipmentHandle = NAME_None;
};
