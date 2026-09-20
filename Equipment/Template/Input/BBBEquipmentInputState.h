#pragma once

#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputPacket.h"

/**
 * 单种装备输入在当前解析帧中的固定槽位
 *
 * 槽位只保留同类输入最后一次提交的数据，不申请堆内存，也不允许输入跨帧残留。
 */
template<typename TPacket>
struct TBBBEquipmentInputSlot final
{
    /**
     * 覆盖当前槽位
     * @param InPacket      本次输入数据
     * @param bInIsMirror   是否来自远端事实还原
     * @return 无
     */
    void Submit(const TPacket &InPacket, const bool bInIsMirror)
    {
        Packet = InPacket;
        bIsMirror = bInIsMirror;
        bActive = true;
    }

    /**
     * 取出当前槽位并立即失活
     * @param OutPacket     接收输入数据
     * @param bOutIsMirror  接收输入执行模式
     * @return 本帧是否存在该输入
     */
    bool Consume(TPacket &OutPacket, bool &bOutIsMirror)
    {
        if (!bActive)
        {
            return false;
        }

        OutPacket = Packet;
        bOutIsMirror = bIsMirror;
        bActive = false;
        return true;
    }

    /** 清除尚未消费的本帧输入 */
    void Reset()
    {
        bActive = false;
        bIsMirror = false;
    }

private:
    /** 本帧是否收到该输入 */
    bool bActive = false;

    /** 本次输入是否只能执行镜像还原 */
    bool bIsMirror = false;

    /** 同类输入最后一次提交的数据 */
    TPacket Packet;
};

/**
 * 所有装备实例共享的固定输入黑板
 *
 * 字段顺序就是唯一解析优先级：事实、装备、副操作、弹匣脱离、弹匣装入、
 * 换弹打断、开始换弹、主操作。新增装备只能实现这些输入的规则，不能改变顺序。
 */
struct FBBBEquipmentInputState final
{
    /** 网络确认事实 */
    TBBBEquipmentInputSlot<FBBBEquipmentFactInput> Fact;

    /** 装备动作 */
    TBBBEquipmentInputSlot<FBBBEquipmentEquipInput> Equip;

    /** 副操作 */
    TBBBEquipmentInputSlot<FBBBEquipmentSecondaryInput> Secondary;

    /** 弹匣脱离 */
    TBBBEquipmentInputSlot<FBBBEquipmentDetachMagazineInput> DetachMagazine;

    /** 弹匣装入 */
    TBBBEquipmentInputSlot<FBBBEquipmentLoadMagazineInput> LoadMagazine;

    /** 换弹打断 */
    TBBBEquipmentInputSlot<FBBBEquipmentInterruptReloadInput> InterruptReload;

    /** 开始换弹 */
    TBBBEquipmentInputSlot<FBBBEquipmentReloadInput> Reload;

    /** 主操作 */
    TBBBEquipmentInputSlot<FBBBEquipmentPrimaryInput> Primary;

    /**
     * 按输入类型覆盖对应固定槽位
     * @param Packet        本次输入数据
     * @param bIsMirror     是否来自远端事实还原
     * @return 无
     */
    template<typename TPacket>
    void Submit(const TPacket &Packet, const bool bIsMirror)
    {
        GetSlot<TPacket>().Submit(Packet, bIsMirror);
    }

    /** 清理全部固定输入槽位 */
    void Reset()
    {
        Fact.Reset();
        Equip.Reset();
        Secondary.Reset();
        DetachMagazine.Reset();
        LoadMagazine.Reset();
        InterruptReload.Reset();
        Reload.Reset();
        Primary.Reset();
    }

private:
    /** @return 输入类型唯一对应的固定槽位 */
    template<typename TPacket>
    TBBBEquipmentInputSlot<TPacket> &GetSlot();
};

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentFactInput> &FBBBEquipmentInputState::GetSlot()
{
    return Fact;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentEquipInput> &FBBBEquipmentInputState::GetSlot()
{
    return Equip;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentSecondaryInput> &FBBBEquipmentInputState::GetSlot()
{
    return Secondary;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentDetachMagazineInput> &FBBBEquipmentInputState::GetSlot()
{
    return DetachMagazine;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentLoadMagazineInput> &FBBBEquipmentInputState::GetSlot()
{
    return LoadMagazine;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentInterruptReloadInput> &FBBBEquipmentInputState::GetSlot()
{
    return InterruptReload;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentReloadInput> &FBBBEquipmentInputState::GetSlot()
{
    return Reload;
}

template<>
inline TBBBEquipmentInputSlot<FBBBEquipmentPrimaryInput> &FBBBEquipmentInputState::GetSlot()
{
    return Primary;
}
