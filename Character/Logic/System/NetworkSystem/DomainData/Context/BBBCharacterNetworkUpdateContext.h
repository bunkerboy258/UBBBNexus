#pragma once

struct FBBBEquipmentNetworkObservationState;
struct FBBBCharacterEquipmentSelectionState;

class UBBBCharacterNetworkComponent;
struct FBBBTraversalNetworkObservationState;
struct FBBBCharacterTraversalState;
struct FBBBAimNetworkObservationState;
struct FBBBAimState;
struct FBBBCharacterLocomotionState;
struct FBBBCharacterNetworkConfig;
struct FBBBCharacterNetworkIdentityState;
struct FBBBCharacterWorldState;
struct FBBBRunNetworkObservationState;
struct FBBBCharacterEquipmentUseState;
struct FBBBCharacterRuntimeData;

/** 本次角色网络观察与发送使用的栈上上下文 */
struct FBBBCharacterNetworkUpdateContext final
{
    /** 瞄准网络观察器跨帧状态 */
    FBBBAimNetworkObservationState &AimObservationState;

    /** 跑步网络观察器跨帧状态 */
    FBBBRunNetworkObservationState &RunObservationState;

    /** 当前机器上的角色网络身份 */
    const FBBBCharacterNetworkIdentityState &NetworkIdentityState;

    /** 当前帧世界时间快照 */
    const FBBBCharacterWorldState &WorldState;

    /** 角色当前瞄准状态 */
    const FBBBAimState &AimState;

    /** 角色当前移动状态 */
    const FBBBCharacterLocomotionState &LocomotionState;

    /** 角色网络发送配置 */
    const FBBBCharacterNetworkConfig &NetworkConfig;

    /** 角色网络传输组件 */
    UBBBCharacterNetworkComponent &NetworkComponent;
    /** 翻越网络观察基准 */
    FBBBTraversalNetworkObservationState &TraversalObservation;
    /** 控制者已经确定的翻越事实 */
    const FBBBCharacterTraversalState &Traversal;
    /** 实际持有关系的发送基准 */
    FBBBEquipmentNetworkObservationState &EquipmentObservation;

    /** 装备系统已经完成的实际关系 */
    const FBBBCharacterEquipmentSelectionState &Equipment;
    /** 装备系统已经成立的使用结果 */
    const FBBBCharacterEquipmentUseState &EquipmentUse;
    /** 本系统观察状态的直接持有链 */
    FBBBCharacterRuntimeData &Data;
};
