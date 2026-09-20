#include "BBBWork/UBBBNexus/Character/Runtime/State/Definition/BBBCharacterNetworkIdentityDefinition.h"

bool FBBBCharacterNetworkIdentityDefinition::HasAuthority() const
{
    return bHasAuthority;
}

bool FBBBCharacterNetworkIdentityDefinition::IsLocallyControlled() const
{
    return bLocallyControlled;
}

EBBBCharacterExecutionMode FBBBCharacterNetworkIdentityDefinition::GetExecutionMode() const
{
    return ExecutionMode;
}

bool FBBBCharacterNetworkIdentityDefinition::IsMirror() const
{
    return ExecutionMode == EBBBCharacterExecutionMode::Mirror;
}

void FBBBCharacterNetworkIdentityDefinition::Refresh(
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
