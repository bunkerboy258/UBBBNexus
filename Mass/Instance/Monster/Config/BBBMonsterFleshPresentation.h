#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterFleshPresentation.generated.h"

class UStaticMesh;
class UMaterialInterface;

/** 少量实体碎肉的外观与落地保留参数 */
USTRUCT(BlueprintType)
struct FBBBMonsterFleshPresentation final
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (DisplayName = "碎肉网格"))
    TObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "碎肉材质"))
    TObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "普通命中数量范围"))
    FIntPoint HitCount = FIntPoint(1, 2);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "严重毁伤数量范围"))
    FIntPoint MajorCount = FIntPoint(3, 5);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "网格缩放范围"))
    FVector2D Scale = FVector2D(0.22, 0.35);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "飞出速度范围", Units = "cm/s"))
    FVector2D Speed = FVector2D(160, 260);

    UPROPERTY(EditAnywhere, meta = (DisplayName = "同目标碎肉间隔", ClampMin = "0.1", Units = "s"))
    float Interval = 0.22f;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "落地保留秒数", ClampMin = "1", ClampMax = "60"))
    float Lifetime = 20;

    UPROPERTY(EditAnywhere, meta = (DisplayName = "淡出秒数", ClampMin = "0.1", ClampMax = "5"))
    float FadeDuration = 2;
};
