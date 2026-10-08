#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceStyleState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceDisplayState.h"
#include "BBBCharacterAppearanceDomainState.generated.h"

/** 角色外观全部状态的唯一直接持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterAppearanceDomainState final
{
    GENERATED_BODY()
public:
    /** @return 最终外观事实 */
    const FBBBCharacterAppearanceSelectionState &ReadAppearanceSelectionState() const
    {
        return SelectionState;
    }
    /** @return 实例参数与基础选择 */
    const FBBBCharacterAppearanceStyleState &ReadAppearanceStyleState() const
    {
        return StyleState;
    }
    /** @return 待消费输入 */
    const FBBBCharacterAppearanceInputState &ReadAppearanceInputState() const
    {
        return InputState;
    }
    /** @return 本地显示结果 */
    const FBBBCharacterAppearanceDisplayState &ReadAppearanceDisplayState() const
    {
        return DisplayState;
    }
private:
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterAppearanceInputProcessor;
    friend class FBBBCharacterAppearanceSelectionProcessor;
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
