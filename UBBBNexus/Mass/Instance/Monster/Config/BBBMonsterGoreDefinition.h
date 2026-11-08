#pragma once

#include "UObject/Object.h"
#include "BBBMonsterDestructionDefinition.h"
#include "BBBMonsterDismembermentDefinition.h"
#include "BBBMonsterGoreDefinition.generated.h"

class UMaterialInterface;
class UTexture2D;
class UBBBMonsterBloodPresentationDefinition;

/** 二级破坏的共享身体资源 毁伤与断肢使用同一呈现入口 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, HideDropdown, meta=(DisplayName="身体二级破坏"))
class ABBB_EVAC_API UBBBMonsterGoreDefinition final : public UObject
{
    GENERATED_BODY()

public:
    /** @return 创建两个同级资源分区 */
    UBBBMonsterGoreDefinition();

    UPROPERTY(EditAnywhere, Category="外观", meta=(DisplayName="完整身体"))
    TObjectPtr<USkeletalMesh> Body;

    UPROPERTY(EditAnywhere, Category="外观", meta=(DisplayName="断点遮罩图集"))
    TObjectPtr<UTexture2D> ProjectionAtlas;

    UPROPERTY(EditAnywhere, Category="外观", meta=(DisplayName="受损身体材质"))
    TArray<TObjectPtr<UMaterialInterface>> Materials;

    UPROPERTY(VisibleAnywhere, Instanced, Category="破坏", meta=(DisplayName="毁伤系统", ShowOnlyInnerProperties))
    TObjectPtr<UBBBMonsterDestructionDefinition> Destruction;

    UPROPERTY(VisibleAnywhere, Instanced, Category="破坏", meta=(DisplayName="断肢系统", ShowOnlyInnerProperties))
    TObjectPtr<UBBBMonsterDismembermentDefinition> Dismemberment;

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="血红颜色"))
    FLinearColor TissueTint = FLinearColor(0.13f, 0.004f, 0.003f);

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="粗糙度", ClampMin="0.1", ClampMax="1"))
    float Roughness = 0.4f;

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="湿润程度", ClampMin="0", ClampMax="1"))
    float Wetness = 0.8f;

    UPROPERTY(EditAnywhere, Category="预算", meta=(DisplayName="每秒喷血上限", ClampMin="1", ClampMax="30"))
    float WoundUpdatesPerSecond = 20;

    /** @return 同一表现配置持有的血液分区 */
    const UBBBMonsterBloodPresentationDefinition* GetBlood() const;

    /** @return 身体 八区毁伤与断点资源是否完整 */
    bool IsReady() const;
};
