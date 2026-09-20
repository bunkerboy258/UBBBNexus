#pragma once

class ABBBCharacter;
class FBBBCharacterInitializer;

/**
 * 角色主更新主管线
 *
 * 在角色 Tick 中统一执行输入解析与各领域系统
 */
class FBBBCharacterUpdate final
{
public:
    /** 执行角色本帧主更新 */
    void Update() const;

private:
    friend class FBBBCharacterInitializer;

    /**
     * 注入主更新所属角色
     * @param InCharacter 目标角色
     */
    void Initialize(ABBBCharacter &InCharacter);

    /** 主更新所属角色 */
    ABBBCharacter *Character = nullptr;
};
