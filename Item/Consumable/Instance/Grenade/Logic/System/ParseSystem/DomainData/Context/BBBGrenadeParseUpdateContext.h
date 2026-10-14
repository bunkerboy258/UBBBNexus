#pragma once
struct FBBBGrenadeRuntimeData;
/** 本次输入解析依赖 */
struct FBBBGrenadeParseUpdateContext final
{
    /** 唯一运行时黑板 */
    FBBBGrenadeRuntimeData &Data;
    /** 是否仅还原远端结果 */
    bool bIsMirror = true;
};
