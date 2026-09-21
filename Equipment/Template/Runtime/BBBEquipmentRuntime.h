#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Runtime/Context/BBBEquipmentRuntimeContext.h"

/** 具体装备实例公开持有的唯一运行时聚合黑板 */
template<typename TSignature>
struct TBBBEquipmentRuntimeData final
{
    /** 具体装备声明的领域状态持有者 */
    typename TSignature::FDomainState Domain;
};

/** 使用具体装备签名执行固定输入顺序的静态运行时 */
template<typename TSignature>
struct TBBBEquipmentRuntime final
{
    using FRuntimeData = TBBBEquipmentRuntimeData<TSignature>;
    using FState = typename TSignature::FState;
    using FDomainState = typename TSignature::FDomainState;

    /**
     * 覆盖输入类型对应的固定槽位
     * @param RuntimeData 当前装备运行时数据根
     * @param Packet 本次输入数据
     * @param bIsMirror 是否执行远端事实还原
     * @return 无
     */
    template<typename TPacket>
    static void SubmitInput(
        FRuntimeData &RuntimeData,
        const TPacket &Packet,
        const bool bIsMirror)
    {
        TBBBEquipmentInputSlot<TPacket> &Slot = GetSlot(
            RuntimeData.Domain.InputState,
            static_cast<TPacket *>(nullptr));
        Slot.Packet = Packet;
        Slot.bIsMirror = bIsMirror;
        Slot.bActive = true;
    }

    /**
     * 根据具体装备配置建立初始状态
     * @param RuntimeData 当前装备运行时数据根
     * @param Definition 具体装备静态配置
     * @return 无
     */
    template<typename TDefinition>
    static void Initialize(FRuntimeData &RuntimeData, const TDefinition &Definition)
    {
        TSignature::Initialize(RuntimeData.Domain.State, Definition);
    }

    /**
     * 依照全局固定优先级消费本帧输入
     * @param RuntimeData 当前装备运行时数据根
     * @param Context 当前装备解析上下文
     * @return 无
     */
    static void Update(FRuntimeData &RuntimeData, FBBBEquipmentRuntimeContext &Context)
    {
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.Fact, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.Equip, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.Secondary, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.DetachMagazine, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.LoadMagazine, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.InterruptReload, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.Reload, Context);
        Dispatch(RuntimeData.Domain.State, RuntimeData.Domain.InputState.Primary, Context);
    }

private:
    static TBBBEquipmentInputSlot<FBBBEquipmentFactInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentFactInput *)
    {
        return State.Fact;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentEquipInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentEquipInput *)
    {
        return State.Equip;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentSecondaryInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentSecondaryInput *)
    {
        return State.Secondary;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentDetachMagazineInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentDetachMagazineInput *)
    {
        return State.DetachMagazine;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentLoadMagazineInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentLoadMagazineInput *)
    {
        return State.LoadMagazine;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentInterruptReloadInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentInterruptReloadInput *)
    {
        return State.InterruptReload;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentReloadInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentReloadInput *)
    {
        return State.Reload;
    }

    static TBBBEquipmentInputSlot<FBBBEquipmentPrimaryInput> &GetSlot(
        FBBBEquipmentInputState &State,
        FBBBEquipmentPrimaryInput *)
    {
        return State.Primary;
    }

    template<typename TPacket>
    static void Dispatch(
        FState &State,
        TBBBEquipmentInputSlot<TPacket> &Slot,
        FBBBEquipmentRuntimeContext &Context)
    {
        if (!Slot.bActive)
        {
            return;
        }

        const TPacket Packet = Slot.Packet;
        const bool bIsMirror = Slot.bIsMirror;
        Slot.bActive = false;
        Slot.bIsMirror = false;

        if (bIsMirror)
        {
            TSignature::Restore(State, Packet, Context);
            return;
        }

        TSignature::Resolve(State, Packet, Context);
    }
};
