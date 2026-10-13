#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "BBBProjectileDefinition.generated.h"

class UMassEntityConfigAsset;
class UNiagaraDataChannelAsset;
class UNiagaraSystem;
class UStaticMesh;

/** 描述一类本地实体弹丸的弹道 伤害与实体模板 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBProjectileDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 每发生成的独立弹丸数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|发射", meta = (ClampMin = "1", ClampMax = "64", DisplayName = "每枪弹丸数 (颗)", ToolTip = "每次开火生成的独立弹丸数 范围 1 到 64 越大弹丸和处理开销越多 每颗独立计伤 武器仍只消耗一发弹药"))
    int32 ProjectilesPerShot = 1;

    /** 相对枪口正前方的散射锥半角 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|发射", meta = (ClampMin = "0.0", ClampMax = "90.0", DisplayName = "散射半角 (度)", ToolTip = "以本机枪口 X 正轴为中心的随机锥半角 越大越分散 0 沿枪口正前方发射"))
    float SpreadHalfAngleDegrees = 0.0f;

    /** 世界重力倍率 零表示直线飞行 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|弹道", meta = (ClampMin = "0.0", DisplayName = "重力倍率", ToolTip = "无单位 乘世界重力 1 为世界重力 0 无重力加速度 越大受重力影响越强"))
    float GravityScale = 0.0f;

    /** 范围伤害半径 零表示点伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (ClampMin = "0.0", ForceUnits = "cm", DisplayName = "爆炸半径 (厘米)", ToolTip = "0 为点伤害弹 大于 0 为范围伤害弹 越大影响范围越大 爆炸弹必须关闭穿透并配置接触引爆或正数引信"))
    float ExplosionRadiusCm = 0.0f;

    /** 爆炸是否也影响发射者 飞行碰撞仍忽略发射者 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (DisplayName = "爆炸伤害发射者", ToolTip = "手雷可对自己和队友造成伤害 不改变普通弹丸的飞行碰撞忽略规则"))
    bool bDamageInstigator = false;

    /** 正数表示出生后延时引爆 零表示没有引信 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (ClampMin = "0.0", ForceUnits = "s", DisplayName = "引信时间 (秒)", ToolTip = "出生后延时引爆的时间 越小越快引爆 0 无计时引信 仅适用于爆炸弹 不能大于最长存活时间"))
    float FuseSeconds = 0.0f;

    /** 范围伤害弹丸接触目标时立即引爆 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (DisplayName = "接触引爆", ToolTip = "仅爆炸弹有效 开启后碰撞立即引爆 并优先于反弹 关闭时必须有正数引信"))
    bool bDetonateOnImpact = true;

    /** 未引爆的弹丸接触后反弹 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (DisplayName = "未引爆时反弹", ToolTip = "仅非接触引爆的爆炸弹有效 开启后碰撞尝试反弹 关闭或反弹后速度过低时停留至引信引爆"))
    bool bBounceOnImpact = false;

    /** 反弹后保留的速度比例 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|爆炸", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "反弹速度保留倍率", ToolTip = "无单位 0 不保留速度 1 保留反射前速度 越大弹得越快 仅未引爆反弹生效"))
    float BounceRestitution = 0.4f;

    /** 批量实例显示的弹体网格 为空时仅显示曳光 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|弹体", meta = (DisplayName = "弹体网格", ToolTip = "可选的批量实例网格 为空时必须配置曳光通道与系统 不作为碰撞或伤害实体"))
    TObjectPtr<UStaticMesh> Mesh = nullptr;

    /** 网格相对飞行方向的旋转 位移和缩放 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|弹体", meta = (DisplayName = "弹体网格局部变换", ToolTip = "相对弹丸飞行姿态的局部变换 X 为前进轴 位移厘米 旋转度 缩放无单位 只改变显示"))
    FTransform MeshRelativeTransform = FTransform::Identity;

    /** 弹丸使用的Mass实体模板与表现配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|实体", meta = (DisplayName = "Mass 实体配置", ToolTip = "创建弹丸实体所用模板 必须含本项目弹丸 Trait 不替代本配置中的发射参数"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig = nullptr;

    /** 枪口初速度 单位厘米每秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|弹道", meta = (ClampMin = "1.0", ForceUnits = "cm/s", DisplayName = "初速度 (厘米/秒)", ToolTip = "枪口发射的速度 越大飞得越快 100 厘米/秒等于 1 米/秒"))
    float InitialSpeedCmPerSecond = 50000.0f;

    /** 弹丸最长存活时间 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|生命周期", meta = (ClampMin = "0.01", ForceUnits = "s", DisplayName = "最长存活时间 (秒)", ToolTip = "出生后最多存在的时间 越大允许飞行或等待引爆越久 命中销毁可以提前结束"))
    float MaximumLifetimeSeconds = 5.0f;

    /** 球形连续扫掠半径 设为零时使用线段检测 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|碰撞", meta = (ClampMin = "0.0", ForceUnits = "cm", DisplayName = "碰撞半径 (厘米)", ToolTip = "沿飞行段连续检测的半径 越大越容易碰到目标 0 使用线段检测 不控制曳光宽度"))
    float CollisionRadiusCm = 2.0f;

    /** 每次命中造成的基础点伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|伤害", meta = (ClampMin = "0.0", DisplayName = "普通伤害 (生命值)", ToolTip = "每颗弹丸对普通 Actor 的点伤害 Mass 部位按普通伤害和耐久伤害混合 爆炸弹使用同一伤害基值"))
    float BaseDamage = 20.0f;

    /** 命中高耐久部位时参与混合的伤害 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|伤害", meta = (ClampMin = "0.0", DisplayName = "耐久伤害 (生命值)", ToolTip = "Mass 部位有效伤害为普通伤害*(1-耐久比例)+耐久伤害*耐久比例 不是额外叠加 普通 Actor 不使用"))
    float DurableDamage = 10.0f;

    /** 一次开火对每个目标的基础踉跄概率 不按弹丸数累乘 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|伤害", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "踉跄值", ToolTip = "0 不触发踉跄 1 为基础必定触发 命中部位再乘部位系数 同发霰弹每个目标只判定一次 不随伤害或穿透衰减"))
    float StaggerValue = 0.25f;

    /** 命中目标后可继续穿过的目标数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|穿透", meta = (ClampMin = "0", ClampMax = "32", DisplayName = "可继续穿透次数", ToolTip = "无单位 0 命中即停止 越大可继续穿过更多 Pawn 或 Mass 小怪 不能穿世界几何 爆炸弹必须为 0"))
    int32 MaximumPenetrations = 0;

    /** 每穿透一个目标后剩余伤害的倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|穿透", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "穿透后伤害保留倍率", ToolTip = "无单位 穿过目标后缩放普通和耐久伤害 1 不衰减 0 后续无伤害 越大后续伤害越高"))
    float PenetrationDamageMultiplier = 0.65f;

    /** 弹丸执行连续碰撞查询使用的碰撞通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|碰撞", meta = (DisplayName = "世界碰撞通道", ToolTip = "查询世界几何及普通 Actor 的通道 Mass 小怪另走实体查询 发射来源和持有角色被忽略"))
    TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;

    /** 批量曳光与命中光效的数据通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|曳光", meta = (DisplayName = "曳光数据通道", ToolTip = "批量发布当前光段位置 长宽和颜色 必须与曳光系统同时配置 空槽和活动槽按固定索引读取"))
    TObjectPtr<UNiagaraDataChannelAsset> PresentationChannel = nullptr;

    /** 子弹共享曳光系统 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|曳光", meta = (DisplayName = "曳光 Niagara 系统", ToolTip = "消费曳光通道的共享系统 必须与通道同时配置 不是每颗弹丸生成一个组件"))
    TObjectPtr<UNiagaraSystem> PresentationSystem = nullptr;

    /** 当帧表面命中批次 空间岛自行启动反馈系统 不保存命中历史 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|命中", meta = (DisplayName = "命中数据通道", ToolTip = "可选的当帧命中反馈通道 未配置则无此批量反馈 不影响碰撞伤害 同一世界的弹丸须共用此通道"))
    TObjectPtr<UNiagaraDataChannelAsset> ImpactChannel = nullptr;

    /** 飞行光段长度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|曳光", meta = (ClampMin = "1.0", ForceUnits = "cm", DisplayName = "曳光最大长度 (厘米)", ToolTip = "从弹丸头部向后延伸的最大光段长度 越大光段越长 出生段限于已经飞过的距离 不伸回枪口后方"))
    float TracerLengthCm = 1000.0f;

    /** 飞行光段宽度 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|曳光", meta = (ClampMin = "0.1", ForceUnits = "cm", DisplayName = "曳光宽度 (厘米)", ToolTip = "光段显示宽度 越大越粗 不改变逻辑碰撞半径"))
    float TracerWidthCm = 2.5f;

    /** 飞行光段的线性发光颜色 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BBB|弹丸|表现|曳光", meta = (DisplayName = "曳光线性颜色", ToolTip = "RGB 为线性发光颜色 可大于 1 A 由特效材质解释 实际亮度还受 Niagara 材质和曝光影响"))
    FLinearColor TracerColor = FLinearColor(20.0f, 8.0f, 1.0f, 1.0f);

    /** @return 配置资产和运行参数是否完整有效 */
    bool IsValid() const;
};
