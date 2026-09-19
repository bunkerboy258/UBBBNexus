#include "BBBWork/UBBBNexus/Character/Input/Packets/_Template/BBBTemplatePacket.h"

// TODO 只在本文件 include 方法体用到的类型 头文件保持最小依赖

bool FBBBTemplatePacket::IsValid() const
{
    // TODO 提交时自检 拒绝非法字段组合
    return true;
}

bool FBBBTemplatePacket::CanExecute(const FBBBCharacterPacketContext &Context) const
{
    // TODO 本帧执行条件 冲突由失败方用 Context.Approved.HasAny 单向声明避让
    return true;
}

void FBBBTemplatePacket::Execute(FBBBCharacterPacketContext &Context) const
{
    // TODO 应用黑板效果
}
