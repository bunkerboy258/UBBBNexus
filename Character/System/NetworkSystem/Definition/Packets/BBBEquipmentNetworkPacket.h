
#pragma once
#include "CoreMinimal.h"
#include "BBBEquipmentNetworkPacket.generated.h"

class UBBBItemDefinition;

//注册到UE反射与序列化系统
USTRUCT()
struct FBBBEquipmentNetworkPacket
{
    //UE 反射系统的必备宏
    GENERATED_BODY()

    UPROPERTY()
    FGuid ItemInstanceId;

    UPROPERTY()
    TObjectPtr<UBBBItemDefinition> ItemDefinition = nullptr;
};
