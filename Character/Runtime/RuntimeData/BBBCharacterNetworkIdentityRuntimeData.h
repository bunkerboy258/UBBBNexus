#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterNetworkIdentityRuntimeData.generated.h"

class FBBBCharacterUpdatePipeline;

/** 角色在当前机器上的玩法执行身份 */
UENUM()
enum class EBBBCharacterExecutionMode : uint8
{
    /** 本机控制角色，可以根据控制输入产生新的玩法事实 */
    Causal,

    /** 非本机控制角色，只能恢复已经由网络确认的玩法事实 */
    Mirror
};

/**
 * 角色在当前机器上的网络身份事实
 *
 * 权威身份只供网络系统决定传输方向，玩法执行身份只由是否本机控制决定。
 * 该事实每帧由角色主管线刷新，领域系统只能读取，禁止自行查询网络角色。
 */
USTRUCT()
struct FBBBCharacterNetworkIdentityRuntimeData
{
    GENERATED_BODY()

public:
    /** @return 当前角色是否由本机持有网络权威 */
    bool HasAuthority() const;

    /** @return 当前角色是否由本机玩家直接控制 */
    bool IsLocallyControlled() const;

    /** @return 当前角色的玩法执行身份 */
    EBBBCharacterExecutionMode GetExecutionMode() const;

    /** @return 当前角色是否只允许恢复网络事实 */
    bool IsMirror() const;

private:
    friend class FBBBCharacterUpdatePipeline;

    /**
     * 刷新本帧身份事实
     * @param bInHasAuthority       当前机器是否持有角色权威
     * @param bInLocallyControlled  当前角色是否由本机玩家控制
     * @return 无
     */
    void Refresh(bool bInHasAuthority, bool bInLocallyControlled);

    /** 当前机器是否持有角色网络权威 */
    bool bHasAuthority = false;

    /** 当前角色是否由本机玩家直接控制 */
    bool bLocallyControlled = false;

    /** 当前角色的玩法执行身份 */
    EBBBCharacterExecutionMode ExecutionMode = EBBBCharacterExecutionMode::Mirror;
};
