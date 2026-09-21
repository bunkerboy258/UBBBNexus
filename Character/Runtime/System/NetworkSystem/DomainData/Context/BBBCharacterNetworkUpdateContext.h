#pragma once

class UBBBCharacterNetworkComponent;
struct FBBBAimNetworkObservationState;
struct FBBBAimState;
struct FBBBCharacterEquipmentEventState;
struct FBBBCharacterEquipmentSelectionState;
struct FBBBCharacterLocomotionState;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterNetworkIdentityState;
struct FBBBCharacterWorldState;
struct FBBBEquipmentNetworkObservationState;
struct FBBBLocomotionNetworkObservationState;

/** 本次角色网络观察与发送使用的栈上上下文 */
struct FBBBCharacterNetworkUpdateContext final
{
    /** 装备网络观察器跨帧状态 */
    FBBBEquipmentNetworkObservationState &EquipmentObservationState;

    /** 瞄准网络观察器跨帧状态 */
    FBBBAimNetworkObservationState &AimObservationState;

    /** 移动网络观察器跨帧状态 */
    FBBBLocomotionNetworkObservationState &LocomotionObservationState;

    /** 当前机器上的角色网络身份 */
    const FBBBCharacterNetworkIdentityState &NetworkIdentityState;

    /** 当前帧世界时间快照 */
    const FBBBCharacterWorldState &WorldState;

    /** 角色当前瞄准状态 */
    const FBBBAimState &AimState;

    /** 角色当前移动状态 */
    const FBBBCharacterLocomotionState &LocomotionState;

    /** 角色当前装备选择状态 */
    const FBBBCharacterEquipmentSelectionState &EquipmentSelectionState;

    /** 本帧已经成立的装备事件 */
    const FBBBCharacterEquipmentEventState &EquipmentEventState;

    /** 角色网络发送配置 */
    const FBBBCharacterNetworkConfig &NetworkConfig;

    /** 角色网络传输组件 */
    UBBBCharacterNetworkComponent &NetworkComponent;
};
