#pragma once

#include "BBBArmDamageDefinition.h"

/** 同一独立左臂的累计组织损伤与当前连通结果 */
struct FBBBArmDamageState final
{
    /** 装配时验证完成的只读采样结构 */
    TSharedPtr<const FBBBArmDamageDefinition> Definition;

    /** 每个采样点的累计损伤 零至一 */
    TArray<float> Damage;

    /** 已经损毁的采样点 重置前保持损毁 */
    TBitArray<> Removed;

    /** 当前仍能连到肩部的采样点 */
    TBitArray<> ShoulderConnected;

    /** 当前仍能连到手部的采样点 */
    TBitArray<> HandConnected;

    /** 这次重置以来已损毁的采样点总数 */
    int32 RemovedCount = 0;

    /** 最近一次命中实际检查的邻近采样点数量 */
    int32 LastVisitedCells = 0;

    /** 最近一次命中新损毁的采样点数量 */
    int32 LastRemovedCells = 0;

    /** 累计接受的有效命中数量 */
    uint32 HitSerial = 0;

    /** 当前至少存在一个有效手部采样点 */
    bool bHandPresent = false;

    /** 当前手部已经与肩部失去连接 */
    bool bHandDetached = false;

private:
    friend struct FBBBArmDamageDomainState;
    FBBBArmDamageState() = default;
};
