#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterNetworkIdentityRuntimeData.h"

void FBBBCharacterNetworkIdentityRuntimeData::Refresh(
    const bool bInHasAuthority,
    const bool bInLocallyControlled)
{
    // 权威身份与玩法因果身份是两条正交事实，不能用 HasAuthority 推导玩法执行权。
    bHasAuthority = bInHasAuthority;
    bLocallyControlled = bInLocallyControlled;
    ExecutionMode = bLocallyControlled
        ? EBBBCharacterExecutionMode::Causal
        : EBBBCharacterExecutionMode::Mirror;
}
