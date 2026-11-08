#pragma once

class ACharacter;
class UCharacterMovementComponent;
struct FBBBCharacterRuntimeData;

/** CMC 后读取移动事实的单次调用上下文 */
struct FBBBCharacterMovementFactContext final
{
    ACharacter &Character;
    const UCharacterMovementComponent &Movement;
    FBBBCharacterRuntimeData &RuntimeData;
    bool bRestoreFacts;
};
