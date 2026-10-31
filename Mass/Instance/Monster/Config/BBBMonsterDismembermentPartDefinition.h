#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterDismembermentPartDefinition.generated.h"

class USkeletalMesh;
class UStaticMesh;

/** 断肢开口与配对组织的制作参数 */
USTRUCT(BlueprintType)
struct FBBBMonsterDismembermentPartDefinition final
{
    GENERATED_BODY()

    /** 稳定资源身份 零保留为未占用 */
    UPROPERTY(EditAnywhere, Category="伤口", meta=(DisplayName="伤口编号", ClampMin="1", ClampMax="255"))
    int32 Id = 1;

    UPROPERTY(EditAnywhere, Category="伤口", meta=(DisplayName="伤口名称"))
    FName Name;

    /** 仅选择预设 不参与身体像素裁剪 */
    UPROPERTY(EditAnywhere, Category="伤口", meta=(DisplayName="适用部位"))
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;

    /** 用于适配和随动 不表示隐藏此骨骼以下身体 */
    UPROPERTY(EditAnywhere, Category="适配", meta=(DisplayName="参考骨骼"))
    FName Bone;

    /** 蒙皮前模型坐标 */
    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="椭球中心", Units="cm"))
    FVector Center = FVector::ZeroVector;

    /** 开口与组织连接位置 不等同于覆盖远端的椭球中心 */
    UPROPERTY(EditAnywhere, Category="适配", meta=(DisplayName="伤口开口位置", Units="cm"))
    FVector Contact = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="椭球朝向"))
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="椭球半尺寸", ClampMin="0.2", Units="cm"))
    FVector Extent = FVector(4, 4, 3);

    /** 配套遮罩图集中的偏移与尺寸 */
    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="遮罩图集区域"))
    FVector4 AtlasRect = FVector4(0, 0, 1, 1);

    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="血层强度", ClampMin="0", ClampMax="1"))
    float BloodStrength = 1;

    /** 人工制作的预制组织 不在预览器生成封盖 */
    UPROPERTY(EditAnywhere, Category="组织", meta=(DisplayName="内部组织模型"))
    TObjectPtr<USkeletalMesh> Tissue;

    /** 可选的脱落预制件 断开时捕获当前姿态 */
    UPROPERTY(EditAnywhere, Category="掉落", meta=(DisplayName="蒙皮掉落件"))
    TObjectPtr<USkeletalMesh> DetachedSkin;

    UPROPERTY(EditAnywhere, Category="掉落", meta=(DisplayName="物理掉落件"))
    TObjectPtr<UStaticMesh> DetachedMesh;

    /** 远端已经脱落后再次近端离断使用的封闭剩余段 */
    UPROPERTY(EditAnywhere, Category="掉落", meta=(DisplayName="剩余段蒙皮掉落件"))
    TObjectPtr<USkeletalMesh> RemainingSkin;

    UPROPERTY(EditAnywhere, Category="掉落", meta=(DisplayName="剩余段物理掉落件"))
    TObjectPtr<UStaticMesh> RemainingMesh;
};
