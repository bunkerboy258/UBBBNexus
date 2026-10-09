#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterSeveredPartDefinition.generated.h"

class UStaticMesh;

/** 已封口的断肢和身体断口静态资源 不持有损毁事实 */
USTRUCT(BlueprintType)
struct FBBBMonsterSeveredPartDefinition final
{
    GENERATED_BODY()

    /** 对应唯一的 Mass 部位编号 */
    UPROPERTY(EditAnywhere, Category = "小怪|断肢", meta = (DisplayName = "损毁部位"))
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Head;

    /** 断开位置的首个骨骼 */
    UPROPERTY(EditAnywhere, Category = "小怪|断肢", meta = (DisplayName = "断开骨骼"))
    FName Bone;

    /** 以断开骨骼为原点的封闭掉落部件 */
    UPROPERTY(EditAnywhere, Category = "小怪|断肢", meta = (DisplayName = "掉落部件"))
    TObjectPtr<UStaticMesh> DetachedMesh;

    /** 同坐标系下独立的身体断口封盖 */
    UPROPERTY(EditAnywhere, Category = "小怪|断肢", meta = (DisplayName = "身体封口"))
    TObjectPtr<UStaticMesh> CapMesh;
};
