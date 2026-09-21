#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Runtime/Context/BBBEquipmentRuntimeContext.h"

/** 具体装备实例私有持有的唯一运行时数据根 */
template<typename TSignature>
struct TBBBEquipmentRuntimeData final
{
    /** 具体装备签名声明的跨帧状态 */
    using FState = typename TSignature::FState;

    /** 具体装备唯一跨帧状态 */
    FState State;

    /** 所有装备共享布局的本帧固定输入 */
    FBBBEquipmentInputState InputState;
};

/**
 * 使用具体装备签名执行固定输入顺序的静态运行时
 *
 * TSignature 必须为八种公共输入分别提供 Resolve 与 Restore。模板直接生成调用，
 * 不使用虚函数、类型查找、动态数组或运行时分配。
 */
template<typename TSignature>
struct TBBBEquipmentRuntime final
{
    /** 具体装备实例的运行时数据根 */
    using FRuntimeData = TBBBEquipmentRuntimeData<TSignature>;

    /**
     * 覆盖输入类型对应的固定槽位
     * @param RuntimeData   当前装备运行时数据根
     * @param Packet        本次输入数据
     * @param bIsMirror     是否执行远端事实还原
     * @return 无
     */
    template<typename TPacket>
    static void SubmitInput(FRuntimeData &RuntimeData, const TPacket &Packet, const bool bIsMirror)
    {
        RuntimeData.InputState.Submit(Packet, bIsMirror);
    }

    /**
     * 让具体签名根据静态配置初始化跨帧状态
     * @param RuntimeData   当前装备运行时数据根
     * @param Definition    具体装备静态配置
     * @return 无
     */
    template<typename TDefinition>
    static void Initialize(FRuntimeData &RuntimeData, const TDefinition &Definition)
    {
        TSignature::Initialize(RuntimeData.State, Definition);
    }

    /**
     * 依照全局固定优先级消费本帧输入
     * @param RuntimeData   当前装备运行时数据根
     * @param Context       当前装备解析上下文
     * @return 无
     */
    static void Update(FRuntimeData &RuntimeData, FBBBEquipmentRuntimeContext &Context)
    {
        Dispatch(RuntimeData.State, RuntimeData.InputState.Fact, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.Equip, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.Secondary, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.DetachMagazine, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.LoadMagazine, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.InterruptReload, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.Reload, Context);
        Dispatch(RuntimeData.State, RuntimeData.InputState.Primary, Context);
        RuntimeData.InputState.Reset();
    }

private:
    /**
     * 按槽位携带的执行模式选择规则判断或事实还原
     * @param State         具体装备跨帧状态
     * @param Slot          当前固定输入槽位
     * @param Context       当前装备解析上下文
     * @return 无
     */
    template<typename TPacket>
    static void Dispatch(
        typename FRuntimeData::FState &State,
        TBBBEquipmentInputSlot<TPacket> &Slot,
        FBBBEquipmentRuntimeContext &Context)
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
};
