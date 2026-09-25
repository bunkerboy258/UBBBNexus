#pragma once

/** 建立装备持有关系后的表现请求 */
struct FBBBEquipmentEquipPacket final
{
    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @return 是否允许写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /** @param State	装备动作输入状态 @return 无 */
    template<typename TState>
    void Apply(TState &State) const
    {
        State.bEquipRequested = true;
    }
};
