#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Runtime/BBBEquipmentRuntimeContext.h"

/**
 * 使用具体装备签名执行固定输入顺序的静态运行时
 *
 * TSignature 必须为八种公共输入分别提供 Resolve 与 Restore。模板直接生成调用，
 * 不使用虚函数、类型查找、动态数组或运行时分配。
 */
template<typename TSignature>
class TBBBEquipmentRuntime final
{
public:
    /** 具体装备签名声明的跨帧状态 */
    using FState = typename TSignature::FState;

    /**
     * 覆盖输入类型对应的固定槽位
     * @param Packet        本次输入数据
     * @param bIsMirror     是否执行远端事实还原
     * @return 无
     */
    template<typename TPacket>
    void SubmitInput(const TPacket &Packet, const bool bIsMirror)
    {
        InputState.Submit(Packet, bIsMirror);
    }

    /**
     * 让具体签名根据静态配置初始化跨帧状态
     * @param Definition    具体装备静态配置
     * @return 无
     */
    template<typename TDefinition>
    void Initialize(const TDefinition &Definition)
    {
        TSignature::Initialize(State, Definition);
    }

    /**
     * 依照全局固定优先级消费本帧输入
     * @param Context       当前装备解析上下文
     * @return 无
     */
    void Update(FBBBEquipmentRuntimeContext &Context)
    {
        Dispatch(InputState.Fact, Context);
        Dispatch(InputState.Equip, Context);
        Dispatch(InputState.Secondary, Context);
        Dispatch(InputState.DetachMagazine, Context);
        Dispatch(InputState.LoadMagazine, Context);
        Dispatch(InputState.InterruptReload, Context);
        Dispatch(InputState.Reload, Context);
        Dispatch(InputState.Primary, Context);
        InputState.Reset();
    }

    /** @return 具体装备当前跨帧状态 */
    const FState &GetState() const
    {
        return State;
    }

private:
    /**
     * 按槽位携带的执行模式选择规则判断或事实还原
     * @param Slot          当前固定输入槽位
     * @param Context       当前装备解析上下文
     * @return 无
     */
    template<typename TPacket>
    void Dispatch(TBBBEquipmentInputSlot<TPacket> &Slot, FBBBEquipmentRuntimeContext &Context)
    {
        TPacket Packet;
        bool bIsMirror = false;
        if (!Slot.Consume(Packet, bIsMirror))
        {
            return;
        }

        if (bIsMirror)
        {
            TSignature::Restore(State, Packet, Context);
            return;
        }

        TSignature::Resolve(State, Packet, Context);
    }

    /** 具体装备唯一跨帧状态 */
    FState State;

    /** 所有装备共享布局的本帧固定输入 */
    FBBBEquipmentInputState InputState;
};
