#pragma once
#include "CoreMinimal.h"
#include "HitReact.h"
#include "BBBCharacterHitReactionComponent.generated.h"

/** 仅提供插件模拟的完整清理 不持有角色生命规则 */
UCLASS(ClassGroup = "BBB", meta = (DisplayName = "角色物理受击"))
class ABBB_EVAC_API UBBBCharacterHitReactionComponent final : public UHitReact
{
    GENERATED_BODY()
  private:
    friend class FBBBCharacterPhysicalPresentationProcessor;
    /** @return 无 清除局部模拟并停止更新 */
    void ClearSimulation();
};
