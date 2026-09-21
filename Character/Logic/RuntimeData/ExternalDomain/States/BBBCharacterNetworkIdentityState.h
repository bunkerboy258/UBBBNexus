#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterNetworkIdentityState.generated.h"

/**
 * 角色在当前机器上的网络身份事实
 *
 * 权威身份只供网络系统决定传输方向，玩法执行身份只由是否本机控制决定。
 * 该事实每帧由角色主管线刷新，领域系统只能读取，禁止自行查询网络角色。
 */
USTRUCT()
struct FBBBCharacterNetworkIdentityState final
{
    GENERATED_BODY()

    /** 当前机器是否持有角色网络权威 */
    bool bHasAuthority = false;

    /** 当前角色是否由本机玩家直接控制 */
    bool bLocallyControlled = false;

    /** 当前角色是否只允许执行镜像还原 */
    bool bIsMirror = true;
};
