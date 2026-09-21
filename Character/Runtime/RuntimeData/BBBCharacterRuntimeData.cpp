#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"

void FBBBCharacterRuntimeData::Clean()
{
    // 收件箱与期望槽位跨帧保留 仅清理已经交给系统消费的帧命令
    Equipment.CleanFrame();
}
