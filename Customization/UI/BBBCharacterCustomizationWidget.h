#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBBCharacterCustomizationWidget.generated.h"

class UBBBCharacterCustomizationController;

/** 换装界面只持有本次操作入口 不直接修改人物组件 */
UCLASS(Abstract, Blueprintable)
class ABBB_EVAC_API UBBBCharacterCustomizationWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 本次换装操作入口 */
    UPROPERTY(BlueprintReadOnly, Category = "BBB|Customization")
    TObjectPtr<UBBBCharacterCustomizationController> Customization;

protected:
    /**
     * 在界面输入模式下处理取消
     * @param Geometry	界面布局
     * @param KeyEvent	按键事件
     * @return 是否消费事件
     */
    virtual FReply NativeOnKeyDown(const FGeometry &Geometry, const FKeyEvent &KeyEvent) override;
};
