#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterGoreWoundDefinition.h"
#include "BBBMonsterGoreDefinition.generated.h"

class UMaterialInterface;
class UTexture2D;
class UBBBMonsterBloodPresentationDefinition;
class UAnimSequence;

/** 正式尸群与预览共用的伤口资源 不生成或修改身体拓扑 */
UCLASS(BlueprintType, meta=(DisplayName="僵尸伤口表现配置"))
class ABBB_EVAC_API UBBBMonsterGoreDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 原有完整身体 不写入断肢标签 */
    UPROPERTY(EditAnywhere, Category="外观", meta=(DisplayName="完整身体"))
    TObjectPtr<USkeletalMesh> Body;

    /** 红通道控制破口 绿通道控制血迹 蓝通道控制局部湿润 */
    UPROPERTY(EditAnywhere, Category="投影", meta=(DisplayName="伤口遮罩图集"))
    TObjectPtr<UTexture2D> ProjectionAtlas;

    /** 与原身体材质槽对应的共享投影材质 */
    UPROPERTY(EditAnywhere, Category="外观", meta=(DisplayName="伤口身体材质"))
    TArray<TObjectPtr<UMaterialInterface>> Materials;

    /** 所有普通伤口和永久破损共用同一资源定义 */
    UPROPERTY(EditAnywhere, EditFixedSize, Category="伤口", meta=(DisplayName="预制伤口", TitleProperty="Name"))
    TArray<FBBBMonsterGoreWoundDefinition> Wounds;

    UPROPERTY(EditAnywhere, Category="血液", meta=(DisplayName="喷血与残留"))
    TObjectPtr<UBBBMonsterBloodPresentationDefinition> Blood;

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="血红颜色"))
    FLinearColor TissueTint = FLinearColor(0.13f, 0.004f, 0.003f);

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="粗糙度", ClampMin="0.1", ClampMax="1"))
    float Roughness = 0.4f;

    UPROPERTY(EditAnywhere, Category="血肉外观", meta=(DisplayName="湿润程度", ClampMin="0", ClampMax="1"))
    float Wetness = 0.8f;

    UPROPERTY(EditAnywhere, Category="命中表现", meta=(DisplayName="基础伤口半径", ClampMin="0.2", ClampMax="8", Units="cm"))
    float HitRadius = 1.8f;

    UPROPERTY(EditAnywhere, Category="预算", meta=(DisplayName="每秒伤口更新上限", ClampMin="1", ClampMax="30"))
    float WoundUpdatesPerSecond = 20;

    UPROPERTY(EditAnywhere, Category="预览", meta=(DisplayName="预览动画"))
    TObjectPtr<UAnimSequence> PreviewAnimation;

    UPROPERTY(EditAnywhere, Category="预览", meta=(DisplayName="自动重置间隔", ClampMin="0", Units="s"))
    float PreviewResetInterval = 3;

    /** @param Id 稳定伤口编号 @return 对应预制伤口或空 */
    const FBBBMonsterGoreWoundDefinition* FindWound(uint8 Id) const;

    /** @return 资源引用与编号是否完整 不以制作标志代替验证 */
    bool IsReady() const;
};
