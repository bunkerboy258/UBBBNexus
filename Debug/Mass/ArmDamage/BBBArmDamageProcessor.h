#pragma once

#include "BBBArmDamageHit.h"
#include "BBBArmDamageRuntimeData.h"

/** 只维护独立左臂的局部累计损伤与当前连接结果 */
struct FBBBArmDamageProcessor final
{
    /**
     * 验证制作结构并建立局部空间查询
     * @param Runtime	独立样板黑板
     * @param Cells	制作侧给出的参考空间采样点与双向连接
     * @return 制作结构有效并已完成装配
     */
    static bool Initialize(FBBBArmDamageRuntimeData& Runtime, TArray<FBBBArmDamageCell> Cells);

    /**
     * 对邻近组织贡献损伤并在结构变化后重新计算连通
     * @param Runtime	独立样板黑板
     * @param Hit	参考空间命中及其毁伤范围
     * @return 接受了有效命中
     */
    static bool ApplyHit(FBBBArmDamageRuntimeData& Runtime, const FBBBArmDamageHit& Hit);

    /**
     * 保留制作结构并恢复全部组织
     * @param Runtime	独立样板黑板
     * @return 无返回值
     */
    static void Reset(FBBBArmDamageRuntimeData& Runtime);

private:
    static void UpdateConnectivity(FBBBArmDamageState& State);
};
