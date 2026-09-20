#include "BBBWork/UBBBNexus/Character/Input/Packets/_Template/BBBTemplatePacket.h"

// TODO: 只在本文件包含方法体真正需要的类型

bool FBBBTemplatePacket::IsValid() const
{
    // TODO: 拒绝非有限数值、无效标识和互相矛盾的字段组合
    return true;
}

bool FBBBTemplatePacket::CanApply(const FBBBCharacterInputContext &Context) const
{
    // TODO: 只读取上下文，冲突规则留在本包内
    return true;
}

void FBBBTemplatePacket::Apply(FBBBCharacterInputContext &Context) const
{
    // TODO: 只修改本包负责的黑板区域
}
