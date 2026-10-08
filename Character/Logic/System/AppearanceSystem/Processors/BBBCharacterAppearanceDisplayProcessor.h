#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterAppearanceDisplayPart;
struct FBBBCharacterAppearanceConfig;
class ABBBCharacter;
struct FBBBCharacterAppearanceUpdateContext;
/** 角色外观的Display职责 */
class FBBBCharacterAppearanceDisplayProcessor final
{
public:
    /** @param Character 正式角色 @param Parts 完整显示结果 @param Config 组件映射 @return 机械显示是否完成 */
    static bool ApplyDisplay(ABBBCharacter &Character, const TArray<FBBBCharacterAppearanceDisplayPart> &Parts,
        const FBBBCharacterAppearanceConfig &Config);
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
    /** @param Context	关闭依赖 @return 无 */
    static void Shutdown(FBBBCharacterAppearanceUpdateContext &Context);
};
