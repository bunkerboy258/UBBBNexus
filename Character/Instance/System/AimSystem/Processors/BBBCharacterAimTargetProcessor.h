#pragma once
struct FBBBCharacterControlState;
struct FBBBAimRuntimeState;

/** 从控制黑板采集世界空间瞄准目标 */
class FBBBCharacterAimTargetProcessor final
{
public:
    /**
     * 更新瞄准目标
     * @param Control	已裁决控制状态
     * @param State	瞄准运行事实
     * @return 无
     */
    void Update(const FBBBCharacterControlState &Control, FBBBAimRuntimeState &State) const;
};
