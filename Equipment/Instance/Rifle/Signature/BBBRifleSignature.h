#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/State/BBBRifleState.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Input/BBBEquipmentInputPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Template/Runtime/BBBEquipmentRuntimeContext.h"

class UBBBRifleDefinition;

/**
 * 步枪对公共装备输入的完整静态实现
 *
 * Resolve 处理本机因果输入并可以产生新事实；Restore 只还原网络已经确认的事实和表现，
 * 禁止生成子弹、后坐力或新的网络事实。所有重载由装备运行时在编译期直接绑定。
 */
struct FBBBRifleSignature final
{
    /** 步枪签名拥有的跨帧状态类型 */
    using FState = FBBBRifleState;

    /**
     * 从步枪配置建立初始状态
     * @param State          待初始化状态
     * @param Definition     步枪静态配置
     * @return 无
     */
    static void Initialize(FState &State, const UBBBRifleDefinition &Definition);

    /** @param State 步枪状态 @param Input 本机事实输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentFactInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像事实输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentFactInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机装备输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentEquipInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像装备输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentEquipInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机副操作输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentSecondaryInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像副操作输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentSecondaryInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机弹匣脱离输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentDetachMagazineInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像弹匣脱离输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentDetachMagazineInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机弹匣装入输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentLoadMagazineInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像弹匣装入输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentLoadMagazineInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机换弹打断输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentInterruptReloadInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像换弹打断输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentInterruptReloadInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机换弹输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentReloadInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像换弹输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentReloadInput &Input, FBBBEquipmentRuntimeContext &Context);

    /** @param State 步枪状态 @param Input 本机主操作输入 @param Context 解析上下文 @return 无 */
    static void Resolve(FState &State, const FBBBEquipmentPrimaryInput &Input, FBBBEquipmentRuntimeContext &Context);
    /** @param State 步枪状态 @param Input 镜像主操作输入 @param Context 解析上下文 @return 无 */
    static void Restore(FState &State, const FBBBEquipmentPrimaryInput &Input, FBBBEquipmentRuntimeContext &Context);
};
