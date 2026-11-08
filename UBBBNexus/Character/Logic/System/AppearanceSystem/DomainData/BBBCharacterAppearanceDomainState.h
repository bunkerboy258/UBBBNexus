#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceStyleState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceDisplayState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterReadabilityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceLoadState.h"
#include "BBBCharacterAppearanceDomainState.generated.h"

/** 角色外观全部状态的唯一直接持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceDomainState final
{
    GENERATED_BODY()
public:
    /** @return 最终外观事实 */
    const FBBBCharacterAppearanceSelectionState &ReadCharacterAppearanceSelectionState() const
    {
        return SelectionState;
    }
    /** @return 实例参数与基础选择 */
    const FBBBCharacterAppearanceStyleState &ReadCharacterAppearanceStyleState() const
    {
        return StyleState;
    }
    /** @return 待消费输入 */
    const FBBBCharacterAppearanceInputState &ReadCharacterAppearanceInputState() const
    {
        return InputState;
    }
    /** @return 本地显示结果 */
    const FBBBCharacterAppearanceDisplayState &ReadCharacterAppearanceDisplayState() const
    {
        return DisplayState;
    }
    /** @return 本地角色可读性显示结果 */
    const FBBBCharacterReadabilityState &ReadCharacterReadabilityState() const
    {
        return ReadabilityState;
    }
    /** @return 当前外观版本的资源请求 */
    const FBBBCharacterAppearanceLoadState &ReadCharacterAppearanceLoadState() const
    {
        return LoadState;
    }
private:
    friend class FBBBCharacterAppearanceLoadProcessor;
    /** 异步加载只在本机维护 不参与外观事实 */
    UPROPERTY()
    FBBBCharacterAppearanceLoadState LoadState;
    friend class FBBBCharacterReadabilityProcessor;
    /** 本地机械补光与模板属性 */
    UPROPERTY()
    FBBBCharacterReadabilityState ReadabilityState;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterAppearanceInputProcessor;
    friend class FBBBCharacterAppearanceSelectionProcessor;
    friend class FBBBCharacterAppearanceResourceProcessor;
    friend class FBBBCharacterAppearanceMaterialProcessor;
    friend class FBBBCharacterAppearanceDisplayProcessor;
    /** 既成事实 */
    UPROPERTY()
    FBBBCharacterAppearanceSelectionState SelectionState;
    /** 参数与基础资源选择 */
    UPROPERTY()
    FBBBCharacterAppearanceStyleState StyleState;
    /** 本机与镜像输入 */
    UPROPERTY()
    FBBBCharacterAppearanceInputState InputState;
    /** 显示执行 */
    UPROPERTY()
    FBBBCharacterAppearanceDisplayState DisplayState;
};
