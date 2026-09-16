#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterReloadAnimationInput.h"
class ABBBCharacterInstance;
class UAnimMontage;
struct FBBBAnimationRuntimeData;
struct FBBBInputRuntimeData;
struct FBBBCharacterEquipmentEvents;
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentActionEvent;

/** 角色外部输入与装备结果入口 */
class ABBB_EVAC_API FBBBCharacterExternalAPI final
{
public:
    /**
     * 提交人物蒙太奇贡献
     * @param Montage	蒙太奇资源
     * @param PlayRate	播放倍率
     * @param Sequence	角色操作序号
     * @param bReload	是否需换弹生命周期回调
     * @return 是否入队
     */
    bool SubmitEquipmentMontage(UAnimMontage *Montage, float PlayRate, int32 Sequence = INDEX_NONE, bool bReload = false);

    /**
     * 提交换弹卸下关键帧
     * @param Sequence	播放时保存的换弹序号
     * @return 无
     */
    void SubmitReloadStartNotify(int32 Sequence);

    /**
     * 提交换弹结束或中断
     * @param Sequence	播放时保存的换弹序号
     * @param EndReason	结束原因
     * @return 无
     */
    void SubmitReloadEndNotify(int32 Sequence, EBBBCharacterReloadEndReason EndReason);

    /**
     * 提交装备已执行事实
     * @param Event	已执行事实
     * @return 无
     */
    void PublishEquipmentEvent(const FBBBEquipmentActionEvent &Event);

    /**
     * 提交本地后坐力
     * @param Impulse	二维后坐力冲量
     * @param RecoverySpeed	恢复速度
     * @return 无
     */
    void SubmitEquipmentRecoil(FVector2D Impulse, float RecoverySpeed);

private:
    friend class FBBBCharacterInitializer;

    /**
     * 注入角色运行依赖
     * @param Character	角色实例
     * @param Animation	动画数据
     * @param Input	输入数据
     * @param Events	装备事件
     * @return 无
     */
    void Initialize(ABBBCharacterInstance &Character, FBBBAnimationRuntimeData &Animation,
        FBBBInputRuntimeData &Input, FBBBCharacterEquipmentEvents &Events,
        FBBBCharacterEquipmentState &EquipmentState);

    void SubmitReloadInput(int32 Sequence, EBBBCharacterReloadAnimationPhase Phase);
    ABBBCharacterInstance *Owner = nullptr;
    FBBBAnimationRuntimeData *AnimationData = nullptr;
    FBBBInputRuntimeData *InputData = nullptr;
    FBBBCharacterEquipmentEvents *EquipmentEvents = nullptr;
    FBBBCharacterEquipmentState *CharacterEquipmentState = nullptr;
};
