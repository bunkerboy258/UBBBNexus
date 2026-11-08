#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/States/BBBWorldDropState.h"
/** 一次解析可修改的业务状态与执行模式 */
struct FBBBWorldDropInputContext final
{
    /** 输入贡献的唯一业务状态 */
    FBBBWorldDropState &Drop;
    /** 是否执行本对象的业务因果 */
    bool bCausal = false;
};
