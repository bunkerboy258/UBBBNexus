#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterDestructionRegionDefinition.generated.h"

/** 一处人工制作的躯干创口 投影与肉壁共用参考姿势坐标 */
USTRUCT(BlueprintType)
struct FBBBMonsterDestructionRegionDefinition final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="区域名称"))
    FName Name;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="参考骨骼"))
    FName Bone;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="开口位置", Units="cm"))
    FVector Contact = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="开口朝向"))
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category="毁伤", meta=(DisplayName="投影半尺寸", Units="cm"))
    FVector Extent = FVector(25, 27, 12);
};
