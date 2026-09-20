#pragma once

#include "BBBWork/UBBBNexus/Character/Core/Update/BBBCharacterLateUpdate.h"

class ABBBCharacter;
class FBBBCharacterInitializer;
class FBBBCharacterShutdown;

/** 角色根管线只选择更新路径并调度阶段 */
class FBBBCharacterUpdatePipeline final
{
private:
    friend class ABBBCharacter;
    friend class FBBBCharacterInitializer;
    friend struct FBBBCharacterLateUpdate;
    friend class FBBBCharacterShutdown;

    /**
     * 在角色全部领域系统完成装配后启用后更新
     *
     * LateUpdate 已在角色注册阶段挂入 Tick 图
     * 此处只在依赖可用后允许它开始执行
     *
     * @param InCharacter 目标角色
     */
    void Initialize(ABBBCharacter &InCharacter);

    /**
     * 注册或注销 CMC 后的角色更新 Tick
     *
     * 注册后形成角色主 Tick 到 CMC 到 LateUpdate 到骨骼网格的依赖链
     *
     * @param InCharacter 目标角色
     * @param bRegister    是否注册 Tick
     */
    void RegisterTickFunctions(ABBBCharacter &InCharacter, bool bRegister);

    /**
     * 执行 CMC 前的角色主更新
     *
     * 顺序固定为时间快照 输入解析 装备 瞄准移动 网络观察
     *
     * @param DeltaSeconds 当前帧间隔
     */
    void Update(float DeltaSeconds) const;

    /** 调度移动完成后的动画事实更新 */
    void LateUpdate() const;

    /** 管线所属角色 */
    ABBBCharacter *Character = nullptr;

    /** CMC 后执行的引擎 Tick 适配器 */
    FBBBCharacterLateUpdate LateUpdateTick;
};
