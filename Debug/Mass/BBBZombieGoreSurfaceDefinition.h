#pragma once

#include "Engine/DataAsset.h"
#include "BBBZombieGoreCutDefinition.h"
#include "BBBZombieWoundCell.h"
#include "BBBZombieGoreSurfaceDefinition.generated.h"

/** 单只调试靶使用的独立身体和断点资源 */
UCLASS(BlueprintType, meta = (DisplayName = "僵尸伤口断点调试配置"))
class ABBB_EVAC_API UBBBZombieGoreSurfaceDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 编辑器重建时使用 不在运行中重新生成几何 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断面制作", meta = (DisplayName = "断面深度倍率", ClampMin = "0.25", ClampMax = "2", ToolTip = "保存后执行调试断面重建生效"))
    float CutDepthScale = 1.0f;

    /** 控制组织块的高低差 不移动身体连接缝 */
    UPROPERTY(EditAnywhere, Category = "血腥调试|断面制作", meta = (DisplayName = "撕裂幅度", ClampMin = "0", ClampMax = "1", ToolTip = "保存后执行调试断面重建生效"))
    float TearStrength = 0.7f;

    /** 与保存后渲染索引对应的预制查询数据 */
    UPROPERTY(VisibleAnywhere, Category = "血腥调试|制作数据", meta = (DisplayName = "表面查询单元"))
    TArray<FBBBZombieWoundCell> WoundCells;

    /** 用于拒绝网格已更换但查询数据尚未重建的情况 */
    UPROPERTY()
    int32 WoundVertexCount = 0;

    /** 当前 LOD0 索引数 */
    UPROPERTY()
    int32 WoundIndexCount = 0;

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
