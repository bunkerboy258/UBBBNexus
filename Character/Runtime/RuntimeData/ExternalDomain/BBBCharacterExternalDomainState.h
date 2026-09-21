#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterWorldState.h"
#include "BBBCharacterExternalDomainState.generated.h"

class FBBBCharacterUpdatePipeline;

/** 角色外部世界与网络身份快照的唯一状态持有者 */
USTRUCT()
struct FBBBCharacterExternalDomainState final
{
    GENERATED_BODY()

public:
    /** @return 当前帧世界快照 */
    const FBBBCharacterWorldState &ReadWorldState() const
    {
        return WorldState;
    }

    /** @return 当前帧网络身份快照 */
    const FBBBCharacterNetworkIdentityState &ReadNetworkIdentityState() const
    {
        return NetworkIdentityState;
    }

private:
    friend class FBBBCharacterUpdatePipeline;

    /** 当前帧世界快照 */
    UPROPERTY(Transient)
    FBBBCharacterWorldState WorldState;

    /** 当前帧网络身份快照 */
    UPROPERTY(Transient)
    FBBBCharacterNetworkIdentityState NetworkIdentityState;
};
