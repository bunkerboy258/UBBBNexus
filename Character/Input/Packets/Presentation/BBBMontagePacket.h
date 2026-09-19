#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterPacketContext.h"

class ABBBCharacter;
class UAnimMontage;

/**
 * 单个固定动画槽位需要的蒙太奇播放数据
 *
 * 具体输入类型只负责声明目标槽位，公共数据和守卫集中在此处避免五份实现产生差异
 */
struct FBBBMontagePacketData
{
    TObjectPtr<UAnimMontage> Montage = nullptr;
    float PlayRate = 1.0f;
    int32 Sequence = INDEX_NONE;
    /** 是否换弹蒙太奇，换弹蒙太奇必须通过当前操作序号守卫 */
    bool bReload = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

protected:
    /**
     * 检查蒙太奇是否允许写入指定槽位
     * @param Context   角色输入上下文
     * @param Slot      目标动画槽位
     * @return 是否允许应用
     */
    bool CanApplyToSlot(const FBBBCharacterPacketContext &Context, FName Slot) const;

    /**
     * 将蒙太奇写入指定槽位
     * @param Context   角色输入上下文
     * @param Slot      目标动画槽位
     */
    void ApplyToSlot(FBBBCharacterPacketContext &Context, FName Slot) const;
};

/** 全身槽位蒙太奇输入 */
struct FBBBFullBodyMontagePacket final : FBBBMontagePacketData
{
    bool CanApply(const FBBBCharacterPacketContext &Context) const;
    void Apply(FBBBCharacterPacketContext &Context) const;
};

/** 上半身槽位蒙太奇输入 */
struct FBBBUpperBodyMontagePacket final : FBBBMontagePacketData
{
    bool CanApply(const FBBBCharacterPacketContext &Context) const;
    void Apply(FBBBCharacterPacketContext &Context) const;
};

/** 瞄准前全身叠加槽位蒙太奇输入 */
struct FBBBFullBodyAdditivePreAimMontagePacket final : FBBBMontagePacketData
{
    bool CanApply(const FBBBCharacterPacketContext &Context) const;
    void Apply(FBBBCharacterPacketContext &Context) const;
};

/** 上半身叠加槽位蒙太奇输入 */
struct FBBBUpperBodyAdditiveMontagePacket final : FBBBMontagePacketData
{
    bool CanApply(const FBBBCharacterPacketContext &Context) const;
    void Apply(FBBBCharacterPacketContext &Context) const;
};

/** 受击叠加槽位蒙太奇输入 */
struct FBBBAdditiveHitReactMontagePacket final : FBBBMontagePacketData
{
    bool CanApply(const FBBBCharacterPacketContext &Context) const;
    void Apply(FBBBCharacterPacketContext &Context) const;
};

/** 武器领域向角色提交蒙太奇的固定槽位翻译入口 */
namespace BBBCharacterMontageInput
{
    /**
     * 按蒙太奇资产中明确配置的轨道分别提交固定槽位输入
     * @param Character  接收表现输入的角色
     * @param Montage    武器领域配置的蒙太奇
     * @param PlayRate   播放倍率
     * @param Sequence   动作操作序号
     * @param bReload    是否属于换弹流程
     * @return 是否识别并提交了全部槽位
     */
    bool Submit(
        ABBBCharacter &Character,
        UAnimMontage &Montage,
        float PlayRate,
        int32 Sequence,
        bool bReload);
}
