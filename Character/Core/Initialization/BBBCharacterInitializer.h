
#pragma once
class ABBBCharacter;
class UInputComponent;
struct FBBBCharacterConfig;

//集中建立角色默认配置输入绑定与运行时依赖
class FBBBCharacterInitializer final
{
    
public:
    
    static void InitializeDefaults(FBBBCharacterConfig &Config);
    static void Initialize(ABBBCharacter &Character);
    static void BindInput(ABBBCharacter &Character, UInputComponent *PlayerInputComponent);
    
};
