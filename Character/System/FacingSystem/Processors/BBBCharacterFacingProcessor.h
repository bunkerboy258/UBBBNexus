#pragma once

class ACharacter;
class UCharacterMovementComponent;
struct FBBBCharacterFacingConfig;
struct FBBBCharacterFacingState;
struct FBBBIntentRuntimeData;

/**
 * 根据移动事实与相机方向更新角色身体朝向和原地转身状态
 */
class FBBBCharacterFacingProcessor final
{
public:
    /**
     * 选择移动对齐、原地等待或原地转身，并配置角色移动组件
     * @param Character		所属角色
     * @param Movement		角色移动组件
     * @param IntentData		只读角色意图
     * @param Config		角色朝向配置
     * @param State		待更新角色朝向状态
     */
    void Update(
        ACharacter &Character,
        UCharacterMovementComponent &Movement,
        const FBBBIntentRuntimeData &IntentData,
        const FBBBCharacterFacingConfig &Config,
        FBBBCharacterFacingState &State) const;
};
