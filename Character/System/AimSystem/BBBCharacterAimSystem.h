
#pragma once
#include "CoreMinimal.h"
class APawn;
class FBBBCharacterInitializer;
struct FBBBAimConfig;
struct FBBBAimRuntimeData;
struct FBBBAimTraceResult;
struct FBBBCharacterAnimationCommands;
struct FBBBCharacterEquipmentState;
struct FBBBIntentRuntimeData;

//计算视线目标骨骼空间瞄准角 与动画目标
class ABBB_EVAC_API FBBBCharacterAimSystem final
{
public:

    /**
     * 每帧更新瞄准状态 计算瞄准目标点与骨骼空间瞄准角并提交运行时数据
     */
    void Update();
private:
    friend class FBBBCharacterInitializer;

    /**
     * 初始化瞄准系统依赖与配置
     * @param InPawn	所属角色Pawn
     * @param InAimData	瞄准运行时数据
     * @param InIntentData	意图运行时数据
     * @param InEquipmentState	装备状态
     * @param InAnimationCommands	动画命令
     * @param InAimConfig	瞄准配置
     */
    void Initialize(
        APawn &InPawn,
        FBBBAimRuntimeData &InAimData,
        const FBBBIntentRuntimeData &InIntentData,
        const FBBBCharacterEquipmentState &InEquipmentState,
        const FBBBCharacterAnimationCommands &InAnimationCommands,
        const FBBBAimConfig &InAimConfig);

    /**
     * 沿相机视线执行瞄准射线检测并写入结果
     * @param ViewLocation	视线起点
     * @param ViewRotation	视线旋转
     * @param EquippedActor	需忽略的当前装备实体
     * @param OutResult	输出瞄准检测结果
     * @return 视线方向有效且结果已写入时返回true
     */
    bool BuildAimTrace(
        const FVector &ViewLocation,
        const FRotator &ViewRotation,
        AActor *EquippedActor,
        FBBBAimTraceResult &OutResult) const;

    APawn *Pawn = nullptr;
    FBBBAimRuntimeData *AimData = nullptr;
    const FBBBIntentRuntimeData *IntentData = nullptr;
    const FBBBCharacterEquipmentState *EquipmentState = nullptr;
    const FBBBCharacterAnimationCommands *AnimationCommands = nullptr;
    const FBBBAimConfig *AimConfig = nullptr;
};
