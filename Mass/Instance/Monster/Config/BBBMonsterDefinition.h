#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterSeveredPartDefinition.h"
#include "BBBMonsterBodyPartDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterDefinition.generated.h"

class UMassEntityConfigAsset;
class UBBBMonsterBloodPresentationDefinition;
class UBBBMonsterSoundPresentationDefinition;
class UBBBMonsterVariationDefinition;

/** 小怪模板与静态玩法参数 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 当前外观的封闭断肢资源 */
    UPROPERTY(EditAnywhere, Category = "BBB|小怪|断肢", meta = (DisplayName = "断肢资源"))
    TArray<FBBBMonsterSeveredPartDefinition> SeveredParts;

    /** @return 建立六个部位的默认静态规则 */
    UBBBMonsterDefinition();

    /** 躯干生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "躯干"))
    FBBBMonsterBodyPartDefinition TorsoPart;

    /** 头部生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "头部"))
    FBBBMonsterBodyPartDefinition HeadPart;

    /** 左臂生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "左臂"))
    FBBBMonsterBodyPartDefinition LeftArmPart;

    /** 右臂生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "右臂"))
    FBBBMonsterBodyPartDefinition RightArmPart;

    /** 左腿生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "左腿"))
    FBBBMonsterBodyPartDefinition LeftLegPart;

    /** 右腿生命与耐久规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪|部位", meta = (DisplayName = "右腿"))
    FBBBMonsterBodyPartDefinition RightLegPart;

    /**
     * @param Part	部位编号
     * @return 对应部位静态规则
     */
    const FBBBMonsterBodyPartDefinition& GetBodyPart(EBBBMonsterHitRegion Part) const;

    /** 重击触发失衡所需的单轮有效伤害比例 */
    UPROPERTY(EditAnywhere, Category = "小怪|受击", meta = (ClampMin = "0.01", ClampMax = "1.0", DisplayName = "重击生命比例"))
    float HeavyHitFraction = 0.22f;

    /** 普通命中每秒压制消退的比例 */
    UPROPERTY(EditAnywhere, Category = "小怪|受击", meta = (ClampMin = "0.01", DisplayName = "压制恢复速率"))
    float SuppressionRecovery = 0.45f;

    /** 单臂损毁后保留的攻击伤害比例 */
    UPROPERTY(EditAnywhere, Category = "小怪|部位", meta = (ClampMin = "0.0", ClampMax = "1.0", DisplayName = "单臂攻击比例"))
    float OneArmAttackRatio = 0.55f;

    /** 行走朝向每秒最大变化角度 */
    UPROPERTY(EditAnywhere, Category = "小怪|移动", meta = (ClampMin = "1.0", DisplayName = "走尸转向速度"))
    float WalkTurnRate = 140.0f;

    /** 奔跑朝向每秒最大变化角度 */
    UPROPERTY(EditAnywhere, Category = "小怪|移动", meta = (ClampMin = "1.0", DisplayName = "跑尸转向速度"))
    float RunTurnRate = 260.0f;

    /** 爬行朝向每秒最大变化角度 */
    UPROPERTY(EditAnywhere, Category = "小怪|移动", meta = (ClampMin = "1.0", DisplayName = "爬行转向速度"))
    float CrawlTurnRate = 90.0f;
    /** 还原出生时使用的实体模板 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (DisplayName = "实体配置"))
    TObjectPtr<UMassEntityConfigAsset> EntityConfig;

    /** 全部外观共享的出生个体化规则 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (DisplayName = "出生差异配置"))
    TObjectPtr<UBBBMonsterVariationDefinition> Variation;

    /** 最大生命值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "最大生命值"))
    float MaxHealth = 100.0f;

    /** 死亡表现保留秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "5.0", DisplayName = "尸体保留时长"))
    float CorpseLifetime = 20.0f;

    /** 未进入物理时死亡动作的独立播放时长 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.1", DisplayName = "死亡动作时长"))
    float DeathAnimationDuration = 1.7f;

    /** 尸体落地后持续物理的最长时长 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.5", DisplayName = "尸体活动物理时长"))
    float CorpseSimulationDuration = 4.0f;

    /** 巡逻与近距离接近速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "走路速度"))
    float WalkSpeed = 100.0f;

    /** 中距离追击速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "跑步速度"))
    float RunSpeed = 300.0f;

    /** 远距离追击速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "冲刺速度"))
    float SprintSpeed = 500.0f;

    /** 正常移动加速度 厘米每平方秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "加速度"))
    float Acceleration = 600.0f;

    /** 正常移动减速度 厘米每平方秒 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "减速度"))
    float Deceleration = 900.0f;

    /** 剩余接近距离进入冲刺的阈值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "冲刺距离阈值"))
    float SprintDistance = 900.0f;

    /** 头部与躯干有效命中的最低速度比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.1", ClampMax = "1.0", DisplayName = "躯干受击速度比例"))
    float BodyHitSpeedRatio = 0.25f;

    /** 头部与躯干减速的平滑恢复秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.01", Units = "s", DisplayName = "躯干减速恢复时间"))
    float BodyHitSlowDuration = 0.6f;

    /** 手臂有效命中的最低速度比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.1", ClampMax = "1.0", DisplayName = "手臂受击速度比例"))
    float ArmHitSpeedRatio = 0.5f;

    /** 手臂减速的平滑恢复秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.01", Units = "s", DisplayName = "手臂减速恢复时间"))
    float ArmHitSlowDuration = 0.5f;

    /** 腿部有效命中的最低速度比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.1", ClampMax = "1.0", DisplayName = "腿部受击速度比例"))
    float LegHitSpeedRatio = 0.15f;

    /** 腿部减速的平滑恢复秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.01", Units = "s", DisplayName = "腿部减速恢复时间"))
    float LegHitSlowDuration = 0.7f;

    /** 命中后保持最低速度的秒数 连射刷新保持阶段 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.0", Units = "s", DisplayName = "受击减速保持时间"))
    float HitSlowHoldDuration = 0.3f;

    /** 轻受击水平停顿秒数 不影响落地和重力 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.0", ClampMax = "0.2", Units = "s", DisplayName = "受击停顿时间"))
    float HitStopDuration = 0.1f;

    /** 站立失衡到恢复的动作秒数 动画按事实采样 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (ClampMin = "0.5", ClampMax = "1.5", Units = "s", DisplayName = "踉跄时间"))
    float StaggerDuration = 0.9f;

    /** 爬行的水平移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|爬行", meta = (ClampMin = "1.0", Units = "cm/s", DisplayName = "爬行速度"))
    float CrawlSpeed = 75.0f;

    /** 爬行逻辑胶囊半高 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|爬行", meta = (ClampMin = "1.0", Units = "cm", DisplayName = "爬行胶囊半高"))
    float CrawlCapsuleHalfHeight = 45.0f;

    /** 腿部失去支撑到完成爬行姿势的秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|爬行", meta = (ClampMin = "0.01", Units = "s", DisplayName = "转入爬行时间"))
    float CrawlTransitionDuration = 1.0f;

    /** 爬行时允许跨越的台阶高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|爬行", meta = (ClampMin = "0.0", Units = "cm", DisplayName = "爬行台阶高度"))
    float CrawlMaxStepHeight = 10.0f;

    /** 血效由实体配置持有 不依赖表现演员是否存在 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|受击", meta = (DisplayName = "血效配置"))
    TObjectPtr<UBBBMonsterBloodPresentationDefinition> BloodPresentation;

    /** 声音只读取 Mass 事实 不影响玩法参数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|声音", meta = (DisplayName = "声音表现配置"))
    TObjectPtr<UBBBMonsterSoundPresentationDefinition> SoundPresentation;

    /** 随机待机时长的下界 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|巡逻", meta = (ClampMin = "0.1", DisplayName = "最短待机时间"))
    float IdleDurationMin = 2.0f;

    /** 随机待机时长的上界 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|巡逻", meta = (ClampMin = "0.1", DisplayName = "最长待机时间"))
    float IdleDurationMax = 4.0f;

    /** 随机巡逻时长的下界 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|巡逻", meta = (ClampMin = "0.1", DisplayName = "最短巡逻时间"))
    float PatrolDurationMin = 2.0f;

    /** 随机巡逻时长的上界 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|巡逻", meta = (ClampMin = "0.1", DisplayName = "最长巡逻时间"))
    float PatrolDurationMax = 5.0f;

    /** 发现或丢失目标后的警觉停留时间 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|巡逻", meta = (ClampMin = "0.1", DisplayName = "警觉时间"))
    float AlertDuration = 1.2f;

    /** 感知范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "视野范围"))
    float SightRange = 3500.0f;

    /** 初次发现的前方视野总夹角 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "1.0", ClampMax = "180.0", DisplayName = "视觉夹角"))
    float SightAngle = 120.0f;

    /** 近距离完整暴露的发现秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "最快视觉确认时间"))
    float SightConfirmMin = 0.3f;

    /** 远距离或部分暴露的发现秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "最慢视觉确认时间"))
    float SightConfirmMax = 1.2f;

    /** 未确认警觉完全消退所需秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "警觉消退时间"))
    float AwarenessDecayDuration = 1.5f;

    /** 无新视觉确认时的追踪上限秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "失去感知追踪时间"))
    float TargetMemoryDuration = 6.0f;

    /** 最后确认位置超过此距离停止追踪 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "1.0", DisplayName = "最大追踪距离"))
    float TargetLeashDistance = 5000.0f;

    /** 新目标相对于当前目标必须达到的距离比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", ClampMax = "0.99", DisplayName = "切换目标距离比例"))
    float TargetSwitchRatio = 0.8f;

    /** 新目标还必须具有的绝对距离优势 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.0", DisplayName = "切换目标距离优势"))
    float TargetSwitchAdvantage = 100.0f;

    /** 新候选持续占优后才切换 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.0", DisplayName = "切换目标确认时间"))
    float TargetSwitchDuration = 0.3f;

    /** 自己视觉确认后向同伴示警的距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.0", DisplayName = "同伴示警范围"))
    float AllyAlertRange = 1200.0f;

    /** 两次同伴示警之间的最短秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "同伴示警间隔"))
    float AllyAlertCooldown = 3.0f;

    /** 到达最后线索位置后的停留秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|感知", meta = (ClampMin = "0.1", DisplayName = "调查结束警觉时间"))
    float InvestigationAlertDuration = 1.5f;

    /** 实际无进展持续此秒数后重算路径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|寻路", meta = (ClampMin = "0.1", DisplayName = "卡住检测时间"))
    float NavigationStuckDuration = 1.5f;

    /** 路径或脱困连续失败次数上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|寻路", meta = (ClampMin = "1", ClampMax = "10", DisplayName = "寻路失败上限"))
    int32 NavigationFailureLimit = 3;

    /** 排除失败目标当前位置的秒数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|寻路", meta = (ClampMin = "0.1", DisplayName = "不可达重试间隔"))
    float NavigationRetryDuration = 4.0f;

    /** 仅在目标附近安排有限包围位置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|寻路", meta = (ClampMin = "1.0", DisplayName = "包围接近启用距离"))
    float EncircleRange = 650.0f;

    /** 逻辑球形碰撞半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "碰撞半径"))
    float CollisionRadius = 45.0f;

    /** 逻辑胶囊中心到脚底的距离 包含端部半球 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "1.0", DisplayName = "胶囊半高"))
    float CapsuleHalfHeight = 90.0f;

    /** 允许跨越的小台阶高度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "0.0", DisplayName = "台阶高度"))
    float MaxStepHeight = 35.0f;

    /** 支撑面的最大可行走倾角 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪|移动", meta = (ClampMin = "0.0", ClampMax = "80.0", DisplayName = "最大坡度"))
    float MaxWalkableSlopeAngle = 50.0f;

    /** 个体间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "个人空间半径"))
    float PersonalSpaceRadius = 110.0f;

    /** 邻居查询范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "邻近搜索半径"))
    float NeighborSearchRadius = 180.0f;

    /** 避让权重 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "避让权重"))
    float AvoidanceWeight = 2.0f;

    /** 攻击距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击范围"))
    float AttackRange = 180.0f;

    /** 攻击伤害 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击伤害"))
    float AttackDamage = 10.0f;

    /** 攻击间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击冷却时间"))
    float AttackCooldown = 1.2f;

    /** 攻击命中前摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击前摇时间"))
    float AttackWindup = 0.35f;

    /** 攻击后摇 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "攻击恢复时间"))
    float AttackRecovery = 0.45f;

    /** 命中对应动画进度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BBB|小怪", meta = (ClampMin = "0.0", DisplayName = "动画命中时间比例"))
    float AnimationHitFraction = 0.4f;


    /** @return 参数是否合法 */
    bool IsValid() const;
};
