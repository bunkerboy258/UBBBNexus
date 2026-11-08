#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterAppearanceUpdateContext;
/** 角色外观的Display职责 */
class FBBBCharacterAppearanceDisplayProcessor final
{
public:
    /** @param Context 本次更新依赖 @return 无 */
    void Update(FBBBCharacterAppearanceUpdateContext &Context) const;
    /** @param Context	关闭依赖 @return 无 */
    static void Shutdown(FBBBCharacterAppearanceUpdateContext &Context);
};
