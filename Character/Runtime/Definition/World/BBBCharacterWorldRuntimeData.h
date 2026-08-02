#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterWorldRuntimeData.generated.h"
class ABBBCharacter;

/** 角色当前帧读取的世界时间快照 */
USTRUCT()
struct FBBBCharacterWorldRuntimeData
{
    GENERATED_BODY()
public:
    /** @return 当前角色帧间隔 */
    float GetFrameDeltaSeconds() const
    {
        return FrameDeltaSeconds;
    }

    /** @return 当前世界游戏时间 */
    float GetWorldTimeSeconds() const
    {
        return WorldTimeSeconds;
    }
private:
    friend class ABBBCharacter;

    /**
     * 更新当前帧世界时间
     * @param InFrameDeltaSeconds 当前角色帧间隔
     * @param InWorldTimeSeconds 当前世界游戏时间
     */
    void Update(float InFrameDeltaSeconds, float InWorldTimeSeconds)
    {
        FrameDeltaSeconds = InFrameDeltaSeconds;
        WorldTimeSeconds = InWorldTimeSeconds;
    }

    /** 当前角色帧间隔 */
    UPROPERTY(Transient)
    float FrameDeltaSeconds = 0.0f;

    /** 当前世界游戏时间 */
    UPROPERTY(Transient)
    float WorldTimeSeconds = 0.0f;
};
