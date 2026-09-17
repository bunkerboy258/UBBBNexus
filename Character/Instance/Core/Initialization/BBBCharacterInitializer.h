
#pragma once
class ABBBCharacter;
class UInputComponent;
struct FBBBCharacterConfig;

//集中建立角色默认配置输入绑定与运行时依赖
class FBBBCharacterInitializer final
{
    
public:
    
    /**
     * 填充角色配置默认值
     * @param Config	待填充的角色配置
     */
    static void InitializeDefaults(FBBBCharacterConfig &Config);
    /**
     * 初始化角色各系统与管线 注入运行时数据与配置
     * @param Character	目标角色
     */
    static void Initialize(ABBBCharacter &Character);
    /**
     * 将配置中的输入动作绑定到增强输入组件
     * @param Character	目标角色
     * @param PlayerInputComponent	玩家输入组件
     */
    static void BindInput(ABBBCharacter &Character, UInputComponent *PlayerInputComponent);
    
};
