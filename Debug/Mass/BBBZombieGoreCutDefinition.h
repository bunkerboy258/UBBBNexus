#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBZombieGoreCutDefinition.generated.h"

class USkeletalMesh;

/** 调试外观的一个预制断点 不持有玩法损毁事实 */
USTRUCT(BlueprintType)
struct FBBBZombieGoreCutDefinition final
{
    GENERATED_BODY()

    /** 所属六部位生命池 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "部位"))
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Head;

    /** 一为近端 二为远端 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "断点档位"))
    int32 Tier = 1;

    /** 断口所属骨骼 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "断口骨骼"))
    FName Bone;

    /** 相对断口骨骼的实际封闭边界中心 用于残端滴血 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "断口局部中心"))
    FVector LocalCenter = FVector::ZeroVector;

    /** 蒙皮姿态下随身体显示的封闭断面 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "身体断面"))
    TObjectPtr<USkeletalMesh> Cap;

    /** 具有独立受限物理资产的掉落部件 */
    UPROPERTY(VisibleAnywhere, meta = (DisplayName = "掉落部件"))
    TObjectPtr<USkeletalMesh> Piece;
};
