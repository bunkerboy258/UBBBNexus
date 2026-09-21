#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterCameraState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterControlState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/States/BBBCharacterOperationState.h"
#include "BBBCharacterParseDomainState.generated.h"

class ABBBCharacter;
class ABBBPlayerCameraSystem;
class FBBBCharacterInputProcessor;
class FBBBCharacterParseSystem;

/** 角色输入解析状态的唯一持有者 */
USTRUCT()
struct FBBBCharacterParseDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色固定输入状态 */
    const FBBBCharacterInputState &ReadInputState() const
    {
        return InputState;
    }

    /** @return 角色跨帧操作状态 */
    const FBBBCharacterOperationState &ReadOperationState() const
    {
        return OperationState;
    }

    /** @return 角色最终控制状态 */
    const FBBBCharacterControlState &ReadControlState() const
    {
        return ControlState;
    }

    /** @return 角色相机表现输入状态 */
    const FBBBCharacterCameraState &ReadCameraState() const
    {
        return CameraState;
    }

private:
    friend class ABBBCharacter;
    friend class ABBBPlayerCameraSystem;
    friend class FBBBCharacterInputProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色固定输入状态 */
    FBBBCharacterInputState InputState;

    /** 输入解析跨帧操作状态 */
    UPROPERTY(Transient)
    FBBBCharacterOperationState OperationState;

    /** 输入系统已经裁决的角色控制状态 */
    UPROPERTY(Transient)
    FBBBCharacterControlState ControlState;

    /** 等待相机系统消费的表现输入 */
    UPROPERTY(Transient)
    FBBBCharacterCameraState CameraState;
};
