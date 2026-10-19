#pragma once

#include "Engine/DataAsset.h"
#include "BBBZombieGoreCutDefinition.h"
#include "BBBZombieGoreSurfaceDefinition.generated.h"

/** 单只调试靶使用的独立身体和断点资源 */
UCLASS(BlueprintType, meta = (DisplayName = "僵尸伤口断点调试配置"))
class ABBB_EVAC_API UBBBZombieGoreSurfaceDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 使用独占伤口图集坐标的自有身体 */
    UPROPERTY(VisibleAnywhere, Category = "血腥调试", meta = (DisplayName = "身体网格"))
    TObjectPtr<USkeletalMesh> Body;

    /** 躯干严重局部破坏使用的小型立体组织凹槽 */
    UPROPERTY(VisibleAnywhere, Category = "血腥调试", meta = (DisplayName = "立体创口"))
    TObjectPtr<class UStaticMesh> HeavyWound;

    /** 头部一个断点 四肢各两个断点 */
    UPROPERTY(VisibleAnywhere, Category = "血腥调试", meta = (DisplayName = "预制断点"))
    TArray<FBBBZombieGoreCutDefinition> Cuts;

    /** 当前结果遮罩的固定边长 不保存逐弹记录 */
    UPROPERTY(VisibleAnywhere, Category = "血腥调试", meta = (DisplayName = "伤口图集边长"))
    int32 AtlasSize = 2048;
};
