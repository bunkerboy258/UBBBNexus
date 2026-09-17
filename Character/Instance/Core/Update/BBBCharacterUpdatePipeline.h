#pragma once
class ABBBCharacter;
class FBBBCharacterInitializer;

/** 角色根管线只选择更新路径并调度阶段 */
class FBBBCharacterUpdatePipeline final
{
public:
    /** 调度角色主更新 */
    void Update() const;
    /** 调度移动完成后的动画事实更新 */
    void LateUpdate() const;

private:
    friend class FBBBCharacterInitializer;
    void Initialize(ABBBCharacter &InCharacter);
    ABBBCharacter *Character = nullptr;
};
