#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/Handling/BBBSniperHandlingSettings.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/Handling/BBBSniperAirborneModifiers.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Config/BBBPlayerCameraRecoilSettings.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Config/Handling/BBBSniperCrouchedModifiers.h"
#include "BBBSniperDefinition.generated.h"

class UAnimMontage;
class UAnimSequence;
class UBBBProjectileDefinition;

/** 狙击枪实例的静态资源与数值配置 */
UCLASS(BlueprintType, PrioritizeCategories = ("物品资料", "装配与握持", "角色动画", "武器动画", "开火与装填", "持枪表现", "镜头后坐力", "专属机制"))
class ABBB_EVAC_API UBBBSniperDefinition final : public UBBBEquipmentDefinition
{
    GENERATED_BODY()

public:
    /** @return 无 为狙击枪配置建立明确类型 */
    UBBBSniperDefinition()
    {
        EquipmentType = EBBBEquipmentType::Sniper;
    }

    /** 固定开火方式 为真时持续请求可连续射击 为假时每次按下只射击一次 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "开火与装填", meta = (DisplayPriority = "1", DisplayName = "按住连续射击", ToolTip = "开启后按住扳机连续请求射击 关闭后每次按下只请求一枪 射速仍由开火间隔限制"))
    bool bAutomaticFire = false;

    /** 左手握持目标所使用的装备插槽 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "4", DisplayName = "左手握持插槽", ToolTip = "武器骨骼网格上的左手握持目标插槽 必须有效 否则取消目标并报警"))
    FName LeftHandSocketName = TEXT("LeftHand");

    /** 左手握持目标在右手骨骼空间中的额外偏移 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "5", DisplayName = "左手位置偏移 (厘米)", ToolTip = "单位厘米 转换插槽位置到角色 hand_r 骨骼空间后再加此偏移 X Y Z 沿 hand_r 的局部轴 正负决定方向"))
    FVector LeftHandIKOffset = FVector::ZeroVector;

    /** 左手握持目标相对装备插槽局部坐标的额外旋转 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "装配与握持", meta = (DisplayPriority = "6", DisplayName = "左手旋转修正 (度)", ToolTip = "单位度 在握持插槽局部空间追加旋转 Pitch 绕 Y 轴 Yaw 绕 Z 轴 Roll 绕 X 轴 再转换到 hand_r 空间"))
    FRotator LeftHandIKRotation = FRotator::ZeroRotator;

    /** 角色装备狙击枪时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "2", DisplayName = "角色装备蒙太奇", ToolTip = "装备时提交给角色播放 使用蒙太奇已注册的角色槽位 不播放在武器网格上"))
    TObjectPtr<UAnimMontage> CharacterEquipMontage = nullptr;

    /** 角色换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "4", DisplayName = "角色换弹蒙太奇", ToolTip = "开始或继续换弹时提交给角色播放 应与本武器换弹动作和通知时序对应"))
    TObjectPtr<UAnimMontage> CharacterReloadMontage = nullptr;

    /** 本武器独立持有的角色局部空间加法后坐力序列 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色动画", meta = (DisplayPriority = "6", DisplayName = "角色后向震动动画", ToolTip = "专属局部空间加法序列 每枪从头求值 按经过秒数播放 强度由持枪配置的后向震动权重控制"))
    TObjectPtr<UAnimSequence> CharacterRecoilAnimation = nullptr;

    /** 狙击枪换弹时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "武器动画", meta = (DisplayPriority = "4", DisplayName = "武器换弹蒙太奇", ToolTip = "播放在本武器骨骼网格上 装弹和操作许可由通知驱动 不用动作时长结算弹药"))
    TObjectPtr<UAnimMontage> EquipmentReloadMontage = nullptr;

    /** 狙击枪开火时播放的蒙太奇 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "武器动画", meta = (DisplayPriority = "2", DisplayName = "武器开火蒙太奇", ToolTip = "每次开火更新在本武器网格播放 同帧多枪不逐个排队 特效和音效由动画通知驱动"))
    TObjectPtr<UAnimMontage> EquipmentFireMontage = nullptr;

    /** 当前狙击枪发射时使用的本地实体弹丸定义 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "开火与装填", meta = (DisplayPriority = "5", DisplayName = "弹丸配置", ToolTip = "决定每次开火的弹丸数量 散射 弹道 伤害与表现 从本机枪口位置和 X 正轴发射"))
    TObjectPtr<UBBBProjectileDefinition> ProjectileDefinition = nullptr;

    /** 各发之后的累计姿态目标 不含起始零点 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "后坐力轨迹", meta = (DisplayName = "逐发轨迹 (度)", ToolTip = "元素 0 对应第一发 X 向上 Y 向右 为累计角度而非单发增量 点数必须等于弹药容量"))
    TArray<FVector2D> RecoilPattern;

    /** 停火后二维姿态角向零靠近的恒定速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "后坐力轨迹", meta = (ClampMin = "0.01", DisplayName = "停火回落速度 (度/秒)", ToolTip = "停火立即回落 二维角度向量每秒缩短此角度 越大回落越快 与 AO 输入平滑和镜头回正独立"))
    float RecoilReturnSpeed = 12.0f;

    /** 弹匣可容纳的子弹数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "开火与装填", meta = (DisplayPriority = "3", ClampMin = "1", DisplayName = "弹药容量 (发)", ToolTip = "单位发 本武器最多装填的弹药数 初始化时装满 越大可连续开火的次数越多"))
    int32 AmmoCapacity = 30;

    /** 连续两次开火的最小时间间隔 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "开火与装填", meta = (DisplayPriority = "2", ClampMin = "0.01", DisplayName = "开火间隔 (秒)", ToolTip = "单位秒 两枪之间的最小间隔 越小射速越快 理论每分钟射速为 60/间隔 实际受帧率和开火规则影响"))
    float FireInterval = 0.2f;

    /** 枪口插槽名称 用于确定发射物生成位置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "开火与装填", meta = (DisplayPriority = "4", DisplayName = "枪口插槽", ToolTip = "武器网格上的发射插槽 位置为弹丸出生点 X 正轴为散射锥中心方向 同时提供瞄准源"))
    FName MuzzleSocketName = TEXT("Muzzle");

    /** 腰射的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "持枪表现|基础姿态", meta = (DisplayPriority = "1", DisplayName = "腰射持枪组", ToolTip = "没有瞄准意图时选择 只提供目标跟随 枪口冲击 后向震动与摇摆参数 镜头另配"))
    FBBBSniperHandlingSettings HipFireSettings;

    /** 瞄准射击的枪口跟随后坐力与摇摆配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "持枪表现|基础姿态", meta = (DisplayPriority = "2", DisplayName = "瞄准持枪组", ToolTip = "具有瞄准意图时选择 在腰射与瞄准组之间直接切换 不按瞄准 Alpha 插值"))
    FBBBSniperHandlingSettings AimFireSettings;

    /** 空中持枪对当前基础配置的修正 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "持枪表现|姿态倍率", meta = (DisplayPriority = "3", DisplayName = "空中倍率组", ToolTip = "角色处于 Falling 时乘到当前基础持枪参数 可以与下蹲倍率叠加 不修改方向冲击回零速率"))
    FBBBSniperAirborneModifiers AirborneModifiers;

    /** 下蹲时对当前腰射或瞄准持枪与镜头配置的倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "持枪表现|姿态倍率", meta = (DisplayPriority = "4", DisplayName = "下蹲倍率组", ToolTip = "角色实际蹲下时叠加 全部为 1 时不变 不修改角色或镜头的角度上限"))
    FBBBSniperCrouchedModifiers CrouchedModifiers;

    /** 腰射时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "镜头后坐力|基础姿态", meta = (DisplayPriority = "1", DisplayName = "腰射镜头组", ToolTip = "相机蓝图按角色瞄准意图选择 只影响镜头冲击 回零及纵向保留 不替代枪口方向冲击"))
    FBBBPlayerCameraRecoilSettings HipFireCameraSettings;

    /** 瞄准射击时贡献给相机的独立配置 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "镜头后坐力|基础姿态", meta = (DisplayPriority = "2", DisplayName = "瞄准镜头组", ToolTip = "具有瞄准意图时选择的镜头配置 不按瞄准 Alpha 插值 与瞄准持枪组独立"))
    FBBBPlayerCameraRecoilSettings AimFireCameraSettings;

    /** 空中相机冲击倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "镜头后坐力|空中倍率", meta = (DisplayPriority = "3", ClampMin = "0.0", DisplayName = "镜头冲击倍率", ToolTip = "无单位 1 不变 越大空中每枪镜头冲击及随机幅度越大 0 无镜头冲击 不缩放角度上限"))
    float AirborneCameraImpulseScale = 1.25f;

    /** 空中相机回零速度倍率 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "镜头后坐力|空中倍率", meta = (DisplayPriority = "4", ClampMin = "0.01", DisplayName = "镜头回零速度倍率", ToolTip = "无单位 1 不变 越大空中镜头回零越快 小于 1 更慢 不改变纵向保留比例"))
    float AirborneCameraRecoveryScale = 0.8f;
};
