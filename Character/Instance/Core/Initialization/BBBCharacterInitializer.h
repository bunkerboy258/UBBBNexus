#pragma once
class ABBBCharacter;

/** 角色系统与管线依赖装配 */
class FBBBCharacterInitializer final
{
public:
    /**
     * 建立角色运行依赖
     * @param Character	目标角色
     * @return 无
     */
    static void Initialize(ABBBCharacter &Character);
};
