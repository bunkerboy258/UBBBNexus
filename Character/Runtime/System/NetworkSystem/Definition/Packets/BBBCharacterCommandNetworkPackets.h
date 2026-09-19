#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterCommandNetworkPackets.generated.h"

/**
 * 本机客户端每帧发送给权威角色的连续控制输入
 *
 * 该结构只负责网络序列化，权威端收到后会还原为正常角色输入包
 */
USTRUCT()
struct FBBBCharacterControlNetworkPacket
{
    GENERATED_BODY()

    UPROPERTY()
    FVector MoveWorld = FVector::ZeroVector;

    UPROPERTY()
    FRotator FacingWorld = FRotator::ZeroRotator;

    UPROPERTY()
    FVector AimTargetWorld = FVector::ZeroVector;

    UPROPERTY()
    bool bMovementActive = false;

    UPROPERTY()
    bool bAimActive = false;

    UPROPERTY()
    bool bWalk = false;

    UPROPERTY()
    bool bSprint = false;

    UPROPERTY()
    bool bCrouch = false;

    UPROPERTY()
    bool bAim = false;

    UPROPERTY()
    bool bFire = false;
};

/**
 * 本机客户端可靠发送给权威角色的离散动作请求
 *
 * 激活标记与输入帧保持一致，同类型动作在发送前仍遵循最后一次覆盖规则
 */
USTRUCT()
struct FBBBCharacterActionNetworkPacket
{
    GENERATED_BODY()

    UPROPERTY()
    int32 EquipSlot = INDEX_NONE;

    UPROPERTY()
    bool bEquipSlot = false;

    UPROPERTY()
    bool bReload = false;
};
